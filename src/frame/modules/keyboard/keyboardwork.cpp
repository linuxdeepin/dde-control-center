/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "keyboardwork.h"
#include "shortcutitem.h"
#include "keyboardmodel.h"
#include <QTime>
#include <QDebug>
#include <QLocale>
#include <QCollator>


namespace dcc {
namespace keyboard{


bool caseInsensitiveLessThan(const MetaData &s1, const MetaData &s2);

KeyboardWorker::KeyboardWorker(KeyboardModel *model, QObject *parent)
    : QObject(parent),
      m_model(model),
      m_keyboardInter(new KeyboardInter("com.deepin.daemon.InputDevices",
                                        "/com/deepin/daemon/InputDevice/Keyboard",
                                        QDBusConnection::sessionBus(), this)),
#ifndef DCC_DISABLE_LANGUAGE
      m_langSelector(new LangSelector("com.deepin.daemon.LangSelector",
                                      "/com/deepin/daemon/LangSelector",
                                      QDBusConnection::sessionBus(), this)),
#endif
      m_keybindInter(new KeybingdingInter("com.deepin.daemon.Keybinding",
                                          "/com/deepin/daemon/Keybinding",
                                          QDBusConnection::sessionBus(), this))
{
    connect(m_keybindInter, SIGNAL(Added(QString,int)), this,SLOT(onAdded(QString,int)));
    connect(m_keybindInter, &KeybingdingInter::Deleted, this, &KeyboardWorker::removed);
#ifndef DCC_DISABLE_KBLAYOUT
    connect(m_keyboardInter, &KeyboardInter::UserLayoutListChanged, this, &KeyboardWorker::onUserLayout);
    connect(m_keyboardInter, &KeyboardInter::CurrentLayoutChanged, this, &KeyboardWorker::onCurrentLayout);
#endif
    connect(m_keyboardInter, SIGNAL(CapslockToggleChanged(bool)), m_model, SLOT(setCapsLock(bool)));
    connect(m_keybindInter, &KeybingdingInter::NumLockStateChanged, m_model, &KeyboardModel::setNumLock);
#ifndef DCC_DISABLE_LANGUAGE
    connect(m_langSelector, &LangSelector::CurrentLocaleChanged, m_model, &KeyboardModel::setLang);
    connect(m_langSelector, &LangSelector::serviceStartFinished, this, [=] {
        QTimer::singleShot(100, this, &KeyboardWorker::onLangSelectorServiceFinished);
    });
#endif
    connect(m_keyboardInter, &KeyboardInter::RepeatDelayChanged, this, &KeyboardWorker::setModelRepeatDelay);
    connect(m_keyboardInter, &KeyboardInter::RepeatIntervalChanged, this, &KeyboardWorker::setModelRepeatInterval);
    connect(m_keybindInter, &KeybingdingInter::ShortcutSwitchLayoutChanged, m_model, &KeyboardModel::setKbSwitch);

    connect(m_keybindInter, &KeybingdingInter::Changed, this, &KeyboardWorker::onShortcutChanged);

    m_keyboardInter->setSync(false);
    m_keybindInter->setSync(false);
#ifndef DCC_DISABLE_LANGUAGE
    m_langSelector->setSync(false, false);
#endif
}

void KeyboardWorker::resetAll() {
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(m_keybindInter->Reset(), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] (QDBusPendingCallWatcher *reply) {
        watcher->deleteLater();

        if (reply->isError()) {
            qWarning() << Q_FUNC_INFO << reply->error();
        }
    });
}

void KeyboardWorker::setShortcutModel(ShortcutModel *model)
{
    m_shortcutModel = model;

    connect(m_keybindInter, &KeybingdingInter::KeyEvent, model, &ShortcutModel::keyEvent);
}

void KeyboardWorker::refreshShortcut()
{
    QDBusPendingCallWatcher *result = new QDBusPendingCallWatcher(m_keybindInter->ListAllShortcuts(), this);
    connect(result, SIGNAL(finished(QDBusPendingCallWatcher*)), this,
            SLOT(onRequestShortcut(QDBusPendingCallWatcher*)));
}

#ifndef DCC_DISABLE_LANGUAGE
void KeyboardWorker::refreshLang()
{
    m_langSelector->blockSignals(false);
    if (!m_langSelector->isValid())
        m_langSelector->startServiceProcess();
    else
        onLangSelectorServiceFinished();
}
#endif

void KeyboardWorker::active()
{
    m_keyboardInter->blockSignals(false);
    m_keybindInter->blockSignals(false);

    setModelRepeatDelay(m_keyboardInter->repeatDelay());
    setModelRepeatInterval(m_keyboardInter->repeatInterval());

    m_metaDatas.clear();
    m_letters.clear();

    Q_EMIT onDatasChanged(m_metaDatas);
    Q_EMIT onLettersChanged(m_letters);

    m_model->setCapsLock(m_keyboardInter->capslockToggle());
    m_model->setNumLock(m_keybindInter->numLockState());

#ifndef DCC_DISABLE_KBLAYOUT
    onRefreshKBLayout();
#endif
#ifndef DCC_DISABLE_LANGUAGE
    refreshLang();
#endif
}

void KeyboardWorker::deactive()
{
    m_keyboardInter->blockSignals(true);
#ifndef DCC_DISABLE_LANGUAGE
    m_langSelector->blockSignals(true);
#endif
    m_keybindInter->blockSignals(true);
}

bool KeyboardWorker::keyOccupy(const QStringList &list)
{
    int bit = 0;
    for (QString t : list) {
        if (t == "Control")
            bit +=  Modifier::control;
        else if (t == "Alt")
            bit += Modifier::alt;
        else if (t == "Super")
            bit += Modifier::super;
        else if (t == "Shift")
            bit += Modifier::shift;
        else
            continue;
    }

    QMap<QStringList,int> keylist = m_model->allShortcut();
    QMap<QStringList, int>::iterator i;
    for (i = keylist.begin(); i != keylist.end(); ++i) {
        if (bit == i.value() && i.key().last() == list.last()) {
            return false;
        }
    }

    return true;
}

#ifndef DCC_DISABLE_KBLAYOUT
void KeyboardWorker::onRefreshKBLayout()
{
    m_model->setKbSwitch(m_keybindInter->shortcutSwitchLayout());

    QDBusPendingCallWatcher *layoutResult = new QDBusPendingCallWatcher(m_keyboardInter->LayoutList(), this);
    connect(layoutResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onLayoutListsFinished);

    onCurrentLayout(m_keyboardInter->currentLayout());
    onUserLayout(m_keyboardInter->userLayoutList());
}
#endif

void KeyboardWorker::modifyShortcutEdit(ShortcutInfo *info)
{
    if (!info)
        return;

    if (info->replace) {
        onDisableShortcut(info->replace);
    }

    const QString &shortcut = info->accels;
    const QString &result = m_keybindInter->LookupConflictingShortcut(shortcut);

    if (!result.isEmpty()) {
        const QJsonObject obj = QJsonDocument::fromJson(result.toLatin1()).object();
        QDBusPendingCall call = m_keybindInter->ClearShortcutKeystrokes(obj["Id"].toString(), obj["Type"].toInt());
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

        watcher->setProperty("id", info->id);
        watcher->setProperty("type", info->type);
        watcher->setProperty("shortcut", shortcut);

        connect(watcher, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onConflictShortcutCleanFinished);
    } else {
        cleanShortcutSlef(info->id, info->type, shortcut);
    }
}

void KeyboardWorker::addCustomShortcut(const QString &name, const QString &command, const QString &accels)
{
    m_keybindInter->AddCustomShortcut(name, command, accels);
}

void KeyboardWorker::modifyCustomShortcut(ShortcutInfo *info)
{
    if (info->replace) {
        onDisableShortcut(info->replace);
    }

    // reset replace shortcut
    info->replace = nullptr;

    const QString &result = m_keybindInter->LookupConflictingShortcut(info->accels);

    if (!result.isEmpty()) {
        const QJsonObject obj = QJsonDocument::fromJson(result.toLatin1()).object();
        QDBusPendingCall call = m_keybindInter->ClearShortcutKeystrokes(obj["Id"].toString(), obj["Type"].toInt());
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

        watcher->setProperty("id", info->id);
        watcher->setProperty("name", info->name);
        watcher->setProperty("command", info->command);
        watcher->setProperty("shortcut", info->accels);

        connect(watcher, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onCustomConflictCleanFinished);
    } else {
        m_keybindInter->ModifyCustomShortcut(info->id, info->name, info->command, info->accels);
    }
}

void KeyboardWorker::grabScreen()
{
    m_keybindInter->GrabScreen();
}

bool KeyboardWorker::checkAvaliable(const QString &key)
{
   const QString &value = m_keybindInter->LookupConflictingShortcut(key);

   return value.isEmpty();
}

void KeyboardWorker::delShortcut(ShortcutInfo* info)
{
    m_keybindInter->DeleteCustomShortcut(info->id);
    m_shortcutModel->delInfo(info);
}

void KeyboardWorker::setRepeatDelay(int value)
{
    m_keyboardInter->setRepeatDelay(converToDBusDelay(value));
}

void KeyboardWorker::setRepeatInterval(int value)
{
    m_keyboardInter->setRepeatInterval(converToDBusInterval(value));
}

void KeyboardWorker::setModelRepeatDelay(int value)
{
    m_model->setRepeatDelay(converToModelDelay(value));
}

void KeyboardWorker::setModelRepeatInterval(int value)
{
    m_model->setRepeatInterval(converToModelInterval(value));
}

void KeyboardWorker::setNumLock(bool value)
{
    m_keybindInter->SetNumLockState(value);
}

void KeyboardWorker::setCapsLock(bool value)
{
    m_keyboardInter->setCapslockToggle(value);
}

void KeyboardWorker::addUserLayout(const QString &value)
{
    m_keyboardInter->AddUserLayout(m_model->kbLayout().key(value));
}

void KeyboardWorker::delUserLayout(const QString &value)
{
    m_keyboardInter->DeleteUserLayout(m_model->userLayout().key(value));
}

bool caseInsensitiveLessThan(const MetaData &s1, const MetaData &s2)
{
    QCollator qc;
    int i = qc.compare(s1.text(), s2.text());
    if (i < 0)
        return true;
    else
        return false;
}

void KeyboardWorker::onRequestShortcut(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;
    if(reply.isError())
    {
        watch->deleteLater();
        return;
    }

    QString info = reply.value();

    QMap<QStringList,int> map;
    QJsonArray array = QJsonDocument::fromJson(info.toStdString().c_str()).array();
    Q_FOREACH(QJsonValue value, array) {
        QJsonObject obj = value.toObject();
        if (obj.isEmpty())
            continue;
        if (obj["Accels"].toArray().isEmpty())
            continue;
        QString accels = obj["Accels"].toArray().at(0).toString();
        accels.replace("<", "");
        accels.replace(">", "-");
        //转换为list
        QStringList key;
        key = accels.split("-");
        int bit = 0;
        for (QString &t : key) {
            if (t == "Control")
                bit += Modifier::control;
            else if (t == "Alt")
                bit += Modifier::alt;
            else if (t == "Super")
                bit += Modifier::super;
            else if (t == "Shift")
                bit += Modifier::shift;
            else {
                QString s = t;
                s = ModelKeycode.value(s);
                if (!s.isEmpty())
                    t = s;
            }
        }
        if (bit == 0)
            continue;

        map.insert(key, bit);
    }
    m_model->setAllShortcut(map);
    m_shortcutModel->onParseInfo(info);
    watch->deleteLater();
}

void KeyboardWorker::onAdded(const QString &in0, int in1)
{
    QDBusPendingReply<QString> reply = m_keybindInter->GetShortcut(in0, in1);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onAddedFinished);
}

void KeyboardWorker::onDisableShortcut(ShortcutInfo *info)
{
    // disable shortcut need wait!
    m_keybindInter->ClearShortcutKeystrokes(info->id, info->type).waitForFinished();
    info->accels.clear();
}

void KeyboardWorker::onAddedFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;

    if (!watch->isError())
        m_shortcutModel->onCustomInfo(reply.value());

    watch->deleteLater();
}

#ifndef DCC_DISABLE_KBLAYOUT
void KeyboardWorker::onLayoutListsFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<KeyboardLayoutList> reply = *watch;

    KeyboardLayoutList tmp_map = reply.value();
    m_model->setLayoutLists(tmp_map);

    watch->deleteLater();
}
#endif

void KeyboardWorker::onLocalListsFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<LocaleList> reply = *watch;

    m_datas.clear();

    LocaleList list = reply.value();

    for (int i = 0; i!=list.size(); ++i) {
        MetaData md;
        md.setKey(list.at(i).id);
        md.setText(list.at(i).name);
        m_datas.append(md);
    }

    qSort(m_datas.begin(), m_datas.end(), caseInsensitiveLessThan);

    m_model->setLocaleList(m_datas);

    watch->deleteLater();
}

void KeyboardWorker::onSetSwitchKBLayout(int value)
{
    m_keybindInter->setShortcutSwitchLayout(value);
}

#ifndef DCC_DISABLE_KBLAYOUT
void KeyboardWorker::onUserLayout(const QStringList &list)
{
    m_model->cleanUserLayout();

    for (const QString &data : list) {
        QDBusPendingCallWatcher *layoutResult = new QDBusPendingCallWatcher(m_keyboardInter->GetLayoutDesc(data), this);
        layoutResult->setProperty("id", data);
        connect(layoutResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onUserLayoutFinished);
    }
}

void KeyboardWorker::onUserLayoutFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;

    m_model->addUserLayout(watch->property("id").toString(), reply.value());

    watch->deleteLater();
}

void KeyboardWorker::onCurrentLayout(const QString &value)
{
    QDBusPendingCallWatcher *layoutResult = new QDBusPendingCallWatcher(m_keyboardInter->GetLayoutDesc(value), this);
    connect(layoutResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onCurrentLayoutFinished);
}

void KeyboardWorker::onCurrentLayoutFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;

    m_model->setLayout(reply.value());

    watch->deleteLater();
}

void KeyboardWorker::onPinyin()
{
    m_letters.clear();
    m_metaDatas.clear();
    QDBusInterface dbus_pinyin("com.deepin.api.Pinyin", "/com/deepin/api/Pinyin",
                               "com.deepin.api.Pinyin");

    Q_FOREACH(const QString & str, m_model->kbLayout().keys()) {
        MetaData md;
        QString title = m_model->kbLayout()[str];
        md.setText(title);
        md.setKey(str);
        QChar letterFirst = title[0];
        QStringList letterFirstList;
        if (letterFirst.isLower() || letterFirst.isUpper()) {
            letterFirstList << QString(letterFirst);
            md.setPinyin(title);
        } else {
            QDBusMessage message = dbus_pinyin.call("Query", title);
            letterFirstList = message.arguments()[0].toStringList();
            md.setPinyin(letterFirstList.at(0));
        }

        append(md);
    }

    QLocale locale;

    if (locale.language() == QLocale::Chinese) {
        QChar ch = '\0';
        for (int i(0); i != m_metaDatas.size(); ++i)
        {
            const QChar flag = m_metaDatas[i].pinyin().at(0).toUpper();
            if (flag == ch)
                continue;
            ch = flag;

            m_letters.append(ch);
            m_metaDatas.insert(i, MetaData(ch, true));
        }
    } else {
        qSort(m_metaDatas.begin(), m_metaDatas.end(), caseInsensitiveLessThan);
    }

    Q_EMIT onDatasChanged(m_metaDatas);
    Q_EMIT onLettersChanged(m_letters);
}

void KeyboardWorker::append(const MetaData &md)
{
    if(m_metaDatas.count() == 0)
    {
        m_metaDatas.append(md);
        return;
    }

    int index = 0;
    for (int i = 0; i != m_metaDatas.size(); ++i) {
        if(m_metaDatas.at(i) > md)
        {
            m_metaDatas.insert(index,md);
            return;
        }
        index++;
    }

    m_metaDatas.append(md);
}
#endif

#ifndef DCC_DISABLE_LANGUAGE
void KeyboardWorker::onLangSelectorServiceFinished()
{
    QDBusPendingCallWatcher *localResult = new QDBusPendingCallWatcher(m_langSelector->GetLocaleList(), this);
    connect(localResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onLocalListsFinished);
    m_langSelector->currentLocale();
}
#endif

void KeyboardWorker::onShortcutChanged(const QString &id, int type)
{
    QDBusPendingCallWatcher *result = new QDBusPendingCallWatcher(m_keybindInter->Query(id, type));
    connect(result, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onGetShortcutFinished);
}

void KeyboardWorker::onGetShortcutFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;

    if (!watch->isError())
        m_shortcutModel->onKeyBindingChanged(reply.value());

    watch->deleteLater();
}

void KeyboardWorker::updateKey(ShortcutInfo *info)
{
    m_shortcutModel->setCurrentInfo(info);

    m_keybindInter->SelectKeystroke();
}

void KeyboardWorker::cleanShortcutSlef(const QString &id, const int type, const QString &shortcut)
{
    QDBusPendingCall call = m_keybindInter->ClearShortcutKeystrokes(id, type);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

    watcher->setProperty("id", id);
    watcher->setProperty("type", type);
    watcher->setProperty("shortcut", shortcut);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onShortcutCleanFinished);
}

void KeyboardWorker::setNewCustomShortcut(const QString &id, const QString &name, const QString &command, const QString &accles)
{
    m_keybindInter->ModifyCustomShortcut(id, name, command, accles);
}

void KeyboardWorker::onConflictShortcutCleanFinished(QDBusPendingCallWatcher *watch)
{
    if (!watch->isError()) {
        const QString &id = watch->property("id").toString();
        const int type = watch->property("type").toInt();
        const QString &shortcut = watch->property("shortcut").toString();

        cleanShortcutSlef(id, type, shortcut);
    }

    watch->deleteLater();
}

void KeyboardWorker::onShortcutCleanFinished(QDBusPendingCallWatcher *watch)
{
    if (!watch->isError()) {
        const QString &id = watch->property("id").toString();
        const int type = watch->property("type").toInt();
        const QString &shortcut = watch->property("shortcut").toString();

        m_keybindInter->AddShortcutKeystroke(id, type, shortcut);
    } else {
        qDebug() << watch->error();
    }

    watch->deleteLater();
}

void KeyboardWorker::onCustomConflictCleanFinished(QDBusPendingCallWatcher *w)
{
    if (!w->isError()) {
        const QString &id = w->property("id").toString();
        const QString name = w->property("name").toString();
        const QString &command = w->property("command").toString();
        const QString &accles = w->property("shortcut").toString();

        setNewCustomShortcut(id, name, command, accles);
    }

    w->deleteLater();
}

int KeyboardWorker::converToDBusDelay(int value)
{
    switch (value) {
    case 1:
        return 20;
    case 2:
        return 80;
    case 3:
        return 150;
    case 4:
        return 250;
    case 5:
        return 360;
    case 6:
        return 480;
    case 7:
        return 600;
    default:
        return 4;
    }
}

int KeyboardWorker::converToModelDelay(int value)
{
    if (value <= 20)
        return 1;
    else if (value <= 80)
        return 2;
    else if (value <= 150)
        return 3;
    else if (value <= 250)
        return 4;
    else if (value <= 360)
        return 5;
    else if (value <= 480)
        return 6;
    else
        return 7;
}

int KeyboardWorker::converToDBusInterval(int value)
{
    switch (value) {
    case 1:
        return 100;
    case 2:
        return 80;
    case 3:
        return 65;
    case 4:
        return 50;
    case 5:
        return 35;
    case 6:
        return 25;
    case 7:
        return 20;
    default:
        return 4;
    }
}

int KeyboardWorker::converToModelInterval(int value)
{
    if (value <= 20)
        return 7;
    else if (value <= 25)
        return 6;
    else if (value <= 35)
        return 5;
    else if (value <= 50)
        return 4;
    else if (value <= 65)
        return 3;
    else if (value <= 80)
        return 2;
    else
        return 1;
}

void KeyboardWorker::setLayout(const QString &value)
{
    m_keyboardInter->setCurrentLayout(value);
}

#ifndef DCC_DISABLE_LANGUAGE
void KeyboardWorker::setLang(const QString &value)
{
    Q_EMIT requestSetAutoHide(false);

    QDBusPendingCall call = m_langSelector->SetLocale(value);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError())
            m_model->setLang(m_langSelector->currentLocale());

        Q_EMIT requestSetAutoHide(true);
        watcher->deleteLater();
    });
}
#endif

}
}
