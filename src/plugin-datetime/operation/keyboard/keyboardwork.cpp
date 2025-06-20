//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#include "keyboardwork.h"

#include <QTimer>
#include <QDebug>
#include <QLocale>
#include <QCollator>
#include <QCoreApplication>
#include <QDBusArgument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDBusInterface>
#include <QDBusPendingCallWatcher>
#include <QTranslator>

using namespace dccV25;
bool caseInsensitiveLessThan(const MetaData &s1, const MetaData &s2);

const QMap<QString, QString> &ModelKeycode = {{"minus", "-"}, {"equal", "="}, {"backslash", "\\"}, {"question", "?/"}, {"exclam", "1"}, {"numbersign", "3"},
    {"semicolon", ";"}, {"apostrophe", "'"}, {"less", ",<"}, {"period", ">."}, {"slash", "?/"}, {"parenleft", "9"}, {"bracketleft", "["},
    {"parenright", "0"}, {"bracketright", "]"}, {"quotedbl", "'"}, {"space", " "}, {"dollar", "$"}, {"plus", "+"}, {"asterisk", "*"},
    {"underscore", "_"}, {"bar", "|"}, {"grave", "`"}, {"at", "2"}, {"percent", "5"}, {"greater", ">."}, {"asciicircum", "6"},
    {"braceleft", "["}, {"colon", ":"}, {"comma", ",<"}, {"asciitilde", "~"}, {"ampersand", "7"}, {"braceright", "]"}, {"Escape", "Esc"}
};


KeyboardWorker::KeyboardWorker(KeyboardModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_keyboardDBusProxy(new KeyboardDBusProxy(this))
    , m_translatorLanguage(nullptr)
{
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::langSelectorServiceStartFinished, this, [=] {
        QTimer::singleShot(100, this, &KeyboardWorker::onLangSelectorServiceFinished);
    });

    m_model->setLangChangedState(m_keyboardDBusProxy->localeState());
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::LocaleStateChanged, m_model, &KeyboardModel::setLangChangedState);

    QMetaObject::invokeMethod(this, "active", Qt::QueuedConnection);
}

void KeyboardWorker::resetAll()
{

}

void KeyboardWorker::onGetWindowWM(bool value)
{
    if (m_shortcutModel)
        m_shortcutModel->onWindowSwitchChanged(value);
}

void KeyboardWorker::setShortcutModel(ShortcutModel *model)
{
    m_shortcutModel = model;
}

void KeyboardWorker::refreshShortcut()
{
}

void KeyboardWorker::refreshLang()
{
    m_keyboardDBusProxy->blockSignals(false);
    if (!m_keyboardDBusProxy->langSelectorIsValid())
        m_keyboardDBusProxy->langSelectorStartServiceProcess();
    else
        onLangSelectorServiceFinished();
}

void KeyboardWorker::windowSwitch()
{
}

void KeyboardWorker::active()
{
    if (!m_translatorLanguage) {
        m_translatorLanguage = new QTranslator(this);
        if (m_translatorLanguage->load(QLocale(), "keyboard_language", "_", TRANSLATE_READ_DIR)) {
            qApp->installTranslator(m_translatorLanguage);
        } else {
            delete m_translatorLanguage;
            m_translatorLanguage = nullptr;
        }
    }

    m_keyboardDBusProxy->blockSignals(false);

    m_metaDatas.clear();
    m_letters.clear();

    Q_EMIT onDatasChanged(m_metaDatas);
    Q_EMIT onLettersChanged(m_letters);

    onRefreshKBLayout();
    refreshLang();
    windowSwitch();
}

void KeyboardWorker::deactive()
{
    m_keyboardDBusProxy->blockSignals(true);
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
}
#endif

void KeyboardWorker::modifyShortcutEditAux(ShortcutInfo *info, bool isKPDelete)
{
    if (!info)
        return;

    if (info->replace) {
        onDisableShortcut(info->replace);
    }

    QString shortcut = info->accels;
    if (!isKPDelete) {
        shortcut = shortcut.replace("KP_Delete", "Delete");
    }
}

void KeyboardWorker::modifyShortcutEdit(ShortcutInfo *info) {
    modifyShortcutEditAux(info);
}

void KeyboardWorker::addCustomShortcut(const QString &name, const QString &command, const QString &accels)
{
    Q_UNUSED(name)
    Q_UNUSED(command)
    Q_UNUSED(accels)
}

void KeyboardWorker::modifyCustomShortcut(ShortcutInfo *info)
{
    if (info->replace) {
        onDisableShortcut(info->replace);
    }
}

void KeyboardWorker::grabScreen()
{
}

bool KeyboardWorker::checkAvaliable(const QString &key)
{
    Q_UNUSED(key)
    return false;
}

void KeyboardWorker::delShortcut(ShortcutInfo* info)
{
    Q_UNUSED(info)
}

void KeyboardWorker::setRepeatDelay(uint value)
{
    Q_UNUSED(value)
}

void KeyboardWorker::setRepeatInterval(uint value)
{
    Q_UNUSED(value)
}

void KeyboardWorker::setModelRepeatDelay(uint value)
{
    m_model->setRepeatDelay(converToModelDelay(value));
}

void KeyboardWorker::setModelRepeatInterval(uint value)
{
    m_model->setRepeatInterval(converToModelInterval(value));
}

void KeyboardWorker::setNumLock(bool value)
{
    Q_UNUSED(value)
}

void KeyboardWorker::setCapsLock(bool value)
{
    Q_UNUSED(value)
}

void KeyboardWorker::addUserLayout(const QString &value)
{
    Q_UNUSED(value)
}

void KeyboardWorker::delUserLayout(const QString &value)
{
    Q_UNUSED(value)
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
    if (m_shortcutModel)
        m_shortcutModel->onParseInfo(info);
    watch->deleteLater();
}

void KeyboardWorker::onAdded(const QString &in0, int in1)
{
    Q_UNUSED(in0)
    Q_UNUSED(in1)
}

void KeyboardWorker::onDisableShortcut(ShortcutInfo *info)
{
    Q_UNUSED(info)
}

void KeyboardWorker::onAddedFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;

    if (m_shortcutModel && !watch->isError())
        m_shortcutModel->onCustomInfo(reply.value());

    watch->deleteLater();
}

void KeyboardWorker::onLayoutListsFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<KeyboardLayoutList> reply = *watch;

    KeyboardLayoutList tmp_map = reply.value();
    m_model->setLayoutLists(tmp_map);

    watch->deleteLater();
}

void KeyboardWorker::onLocalListsFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<LocaleList> reply = *watch;

    m_datas.clear();

    LocaleList list = reply.value();

    for (int i = 0; i!=list.size(); ++i) {
        MetaData md;
        md.setKey(list.at(i).id);
        md.setText(QString("%1 - %2").arg(list.at(i).name).arg(QCoreApplication::translate("dcc::keyboard::Language",list.at(i).name.toUtf8().data())));
        m_datas.append(md);
    }

    std::sort(m_datas.begin(), m_datas.end(), caseInsensitiveLessThan);

    m_model->setLocaleList(m_datas);

    watch->deleteLater();

    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::CurrentLocaleChanged, m_model, &KeyboardModel::setLang);
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::LocalesChanged, m_model, &KeyboardModel::setLocaleLang);
    m_model->setLocaleLang(m_keyboardDBusProxy->locales());
    m_model->setLang(m_keyboardDBusProxy->currentLocale());
}

void KeyboardWorker::onUserLayout(const QStringList &list)
{
    m_model->cleanUserLayout();
    m_model->getUserLayoutList() = list;
}

void KeyboardWorker::onUserLayoutFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;

    m_model->addUserLayout(watch->property("id").toString(), reply.value());

    watch->deleteLater();
}

void KeyboardWorker::onCurrentLayout(const QString &value)
{
    Q_UNUSED(value)
}

void KeyboardWorker::onSearchShortcuts(const QString &searchKey)
{
    Q_UNUSED(searchKey)
}

void KeyboardWorker::onCurrentLayoutFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;

    m_model->setLayout(reply.value());

    watch->deleteLater();
}

void KeyboardWorker::onSearchFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;
    if (m_shortcutModel && !watch->isError()) {
        m_shortcutModel->setSearchResult(reply.value());
    } else {
        qDebug() << "search finished error." << watch->error();
    }
    watch->deleteLater();
}

void KeyboardWorker::onPinyin()
{
    m_letters.clear();
    m_metaDatas.clear();
    QDBusInterface dbus_pinyin("org.deepin.dde.Pinyin1", "/org/deepin/dde/Pinyin1",
                               "org.deepin.dde.Pinyin1");

    Q_FOREACH(const QString &str, m_model->kbLayout().keys()) {
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
        std::sort(m_metaDatas.begin(), m_metaDatas.end(), caseInsensitiveLessThan);
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

void KeyboardWorker::onLangSelectorServiceFinished()
{
    QDBusPendingCallWatcher *localResult = new QDBusPendingCallWatcher(m_keyboardDBusProxy->GetLocaleList(), this);
    connect(localResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onLocalListsFinished);
    m_keyboardDBusProxy->currentLocale();
}

void KeyboardWorker::onShortcutChanged(const QString &id, int type)
{
    Q_UNUSED(id)
    Q_UNUSED(type)
}

void KeyboardWorker::onGetShortcutFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;

    if (m_shortcutModel && !watch->isError())
        m_shortcutModel->onKeyBindingChanged(reply.value());

    watch->deleteLater();
}

void KeyboardWorker::updateKey(ShortcutInfo *info)
{
    Q_UNUSED(info)
}

void KeyboardWorker::cleanShortcutSlef(const QString &id, const int type, const QString &shortcut)
{
    Q_UNUSED(id)
    Q_UNUSED(type)
    Q_UNUSED(shortcut)
}

void KeyboardWorker::setNewCustomShortcut(const QString &id, const QString &name, const QString &command, const QString &accles)
{
    Q_UNUSED(id)
    Q_UNUSED(name)
    Q_UNUSED(command)
    Q_UNUSED(accles)
}

void KeyboardWorker::onConflictShortcutCleanFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
}

void KeyboardWorker::onShortcutCleanFinished(QDBusPendingCallWatcher *watch)
{
    Q_UNUSED(watch)
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

uint KeyboardWorker::converToDBusDelay(uint value)
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

uint KeyboardWorker::converToModelDelay(uint value)
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

uint KeyboardWorker::converToModelInterval(uint value)
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
    Q_UNUSED(value)
}

void KeyboardWorker::setLang(const QString &value)
{
    Q_EMIT requestSetAutoHide(false);

    QDBusPendingCall call = m_keyboardDBusProxy->SetLocale(value);
    qDebug() << "setLang is " << value;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            qDebug() << "setLang error: " << call.error().type();
            m_model->setLang(m_keyboardDBusProxy->currentLocale());
        }

        qDebug() << "setLang success";
        Q_EMIT requestSetAutoHide(true);
        watcher->deleteLater();
    });
}

void KeyboardWorker::addLang(const QString &value)
{
    Q_EMIT requestSetAutoHide(false);

    QDBusPendingCall call = m_keyboardDBusProxy->AddLocale(value);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            qDebug() << "add Locale language error: " << call.error().type();
        }

        Q_EMIT requestSetAutoHide(true);
        watcher->deleteLater();
    });
}

void KeyboardWorker::deleteLang(const QString &value)
{
    Q_EMIT requestSetAutoHide(false);

    QString lang = m_model->langFromText(value);
    QDBusPendingCall call = m_keyboardDBusProxy->DeleteLocale(lang);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            qDebug() << "delete Locale language error: " << call.error().type();
        }

        Q_EMIT requestSetAutoHide(true);
        watcher->deleteLater();
    });
}
