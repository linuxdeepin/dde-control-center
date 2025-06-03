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

using namespace DTK_NAMESPACE::Core;
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
    , m_inputDevCfg(DConfig::create("org.deepin.dde.daemon", "org.deepin.dde.daemon.inputdevices", QString(), this))
{
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::compositingEnabledChanged, this, &KeyboardWorker::onGetWindowWM);
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::Added, this, &KeyboardWorker::onAdded);
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::Deleted, this, &KeyboardWorker::removed);
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::UserLayoutListChanged, this, &KeyboardWorker::onUserLayout);
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::CurrentLayoutChanged, this, &KeyboardWorker::onCurrentLayout);
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::CapslockToggleChanged, m_model, &KeyboardModel::setCapsLock);
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::NumLockStateChanged, m_model, &KeyboardModel::setNumLock);
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::langSelectorServiceStartFinished, this, [=] {
        QTimer::singleShot(100, this, &KeyboardWorker::onLangSelectorServiceFinished);
    });
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::RepeatDelayChanged, this, &KeyboardWorker::setModelRepeatDelay);
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::RepeatIntervalChanged, this, &KeyboardWorker::setModelRepeatInterval);

    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::Changed, this, &KeyboardWorker::onShortcutChanged);

    m_model->setLangChangedState(m_keyboardDBusProxy->localeState());
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::LocaleStateChanged, m_model, &KeyboardModel::setLangChangedState);
}

void KeyboardWorker::resetAll() {
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(m_keyboardDBusProxy->KeybindingReset(), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] (QDBusPendingCallWatcher *reply) {
        watcher->deleteLater();

        if (reply->isError()) {
            qDebug() << Q_FUNC_INFO << reply->error();
        }

        Q_EMIT onResetFinished();
        // reset 之后主动更新快捷键。。。
        refreshShortcut();
    });
}

void KeyboardWorker::onGetWindowWM(bool value)
{
    windowSwitch();
}

void KeyboardWorker::setShortcutModel(ShortcutModel *model)
{
    m_shortcutModel = model;

    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::KeyEvent, model, &ShortcutModel::keyEvent);
}

void KeyboardWorker::refreshShortcut()
{
    QDBusPendingCallWatcher *result = new QDBusPendingCallWatcher(m_keyboardDBusProxy->ListAllShortcuts(), this);
    connect(result, SIGNAL(finished(QDBusPendingCallWatcher*)), this,
            SLOT(onRequestShortcut(QDBusPendingCallWatcher*)));
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
    QDBusInterface licenseInfo("com.deepin.wm",
                               "/com/deepin/wm",
                               "com.deepin.wm",
                               QDBusConnection::sessionBus());
    if (!licenseInfo.isValid()) {
        qDebug() << "com.deepin.license error ," << licenseInfo.lastError().name();
        return;
    }

    if (m_shortcutModel)
        m_shortcutModel->onWindowSwitchChanged(licenseInfo.property("compositingEnabled").toBool());
}

void KeyboardWorker::active()
{
    if (!m_translatorLanguage) {
        m_translatorLanguage = new QTranslator(this);
        if (m_translatorLanguage->load(QLocale(), "keyboard_language", TRANSLATE_READ_DIR)) {
            qApp->installTranslator(m_translatorLanguage);
        } else {
            delete m_translatorLanguage;
            m_translatorLanguage = nullptr;
        }
    }

    m_keyboardDBusProxy->blockSignals(false);

    setModelRepeatDelay(m_keyboardDBusProxy->repeatDelay());
    setModelRepeatInterval(m_keyboardDBusProxy->repeatInterval());

    m_metaDatas.clear();
    m_letters.clear();

    Q_EMIT onDatasChanged(m_metaDatas);
    Q_EMIT onLettersChanged(m_letters);

    m_model->setCapsLock(m_keyboardDBusProxy->capslockToggle());
    m_model->setNumLock(m_keyboardDBusProxy->numLockState());

    onRefreshKBLayout();
    refreshLang();
    windowSwitch();
    refreshShortcut();
    if (m_inputDevCfg->isValid()) {
        QMetaObject::invokeMethod(m_model, "setKeyboardEnabled", Qt::DirectConnection, Q_ARG(bool, m_inputDevCfg->value("keyboardEnabled").toBool()));
        connect(m_inputDevCfg, &DConfig::valueChanged, this, [=](QString key) {
            if (key == "keyboardEnabled") {
                QMetaObject::invokeMethod(m_model, "setKeyboardEnabled", Qt::DirectConnection, Q_ARG(bool, m_inputDevCfg->value(key).toBool()));
            }
        });
    } else {
        qWarning() << QString("DConfig is invalide, name:[%1], subpath[%2].").arg(m_inputDevCfg->name(), m_inputDevCfg->subpath());
    }
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
    QDBusPendingCallWatcher *layoutResult = new QDBusPendingCallWatcher(m_keyboardDBusProxy->LayoutList(), this);
    connect(layoutResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onLayoutListsFinished);

    onCurrentLayout(m_keyboardDBusProxy->currentLayout());
    onUserLayout(m_keyboardDBusProxy->userLayoutList());
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

    const QString &result = m_keyboardDBusProxy->LookupConflictingShortcut(shortcut);

    if (!result.isEmpty()) {
        const QJsonObject obj = QJsonDocument::fromJson(result.toLatin1()).object();
        QDBusPendingCall call = m_keyboardDBusProxy->ClearShortcutKeystrokes(obj["Id"].toString(), obj["Type"].toInt());
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

        watcher->setProperty("id", info->id);
        watcher->setProperty("type", info->type);
        watcher->setProperty("shortcut", shortcut);
        watcher->setProperty("clean", !isKPDelete);

        connect(watcher, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onConflictShortcutCleanFinished);
    } else {
        if (isKPDelete) {
            m_keyboardDBusProxy->AddShortcutKeystroke(info->id, static_cast<int>(info->type), shortcut);
        } else {
            cleanShortcutSlef(info->id, static_cast<int>(info->type), shortcut);
        }
    }
}

void KeyboardWorker::modifyShortcutEdit(ShortcutInfo *info) {
    modifyShortcutEditAux(info);
}

void KeyboardWorker::addCustomShortcut(const QString &name, const QString &command, const QString &accels)
{
    m_keyboardDBusProxy->AddCustomShortcut(name, command, accels);
}

void KeyboardWorker::modifyCustomShortcut(ShortcutInfo *info)
{
    if (info->replace) {
        onDisableShortcut(info->replace);
    }

    // reset replace shortcut
    info->replace = nullptr;

    const QString &result = m_keyboardDBusProxy->LookupConflictingShortcut(info->accels);

    if (!result.isEmpty()) {
        const QJsonObject obj = QJsonDocument::fromJson(result.toUtf8()).object();
        QDBusPendingCall call = m_keyboardDBusProxy->ClearShortcutKeystrokes(obj["Id"].toString(), obj["Type"].toInt());
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

        watcher->setProperty("id", info->id);
        watcher->setProperty("name", info->name);
        watcher->setProperty("command", info->command);
        watcher->setProperty("shortcut", info->accels);

        connect(watcher, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onCustomConflictCleanFinished);
    } else {
        m_keyboardDBusProxy->ModifyCustomShortcut(info->id, info->name, info->command, info->accels);
    }
}

void KeyboardWorker::grabScreen()
{
    m_keyboardDBusProxy->GrabScreen();
}

bool KeyboardWorker::checkAvaliable(const QString &key)
{
   const QString &value = m_keyboardDBusProxy->LookupConflictingShortcut(key);

   return value.isEmpty();
}

void KeyboardWorker::delShortcut(ShortcutInfo* info)
{
    m_keyboardDBusProxy->DeleteCustomShortcut(info->id);
    if (m_shortcutModel)
        m_shortcutModel->delInfo(info);
}

void KeyboardWorker::setRepeatDelay(uint value)
{
    m_keyboardDBusProxy->setRepeatDelay(converToDBusDelay(value));
}

void KeyboardWorker::setRepeatInterval(uint value)
{
    m_keyboardDBusProxy->setRepeatInterval(static_cast<uint>(converToDBusInterval(value)));
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
    m_keyboardDBusProxy->SetNumLockState(value);
}

void KeyboardWorker::setCapsLock(bool value)
{
    m_keyboardDBusProxy->setCapslockToggle(value);
}

void KeyboardWorker::setKeyboardEnabled(bool value)
{
    if (m_inputDevCfg->isValid()) {
        m_inputDevCfg->setValue("keyboardEnabled", value);
    }
}

void KeyboardWorker::addUserLayout(const QString &value)
{
    m_keyboardDBusProxy->AddUserLayout(m_model->kbLayout().key(value));
}

void KeyboardWorker::delUserLayout(const QString &value)
{
    m_keyboardDBusProxy->DeleteUserLayout(m_model->userLayout().key(value));
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
    QDBusPendingReply<QString> reply = m_keyboardDBusProxy->GetShortcut(in0, in1);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onAddedFinished);
}

void KeyboardWorker::onDisableShortcut(ShortcutInfo *info)
{
    // disable shortcut need wait!
    m_keyboardDBusProxy->ClearShortcutKeystrokes(info->id, static_cast<int>(info->type)).waitForFinished();
    info->accels.clear();
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

    for (const QString &data : list) {
        QDBusPendingCallWatcher *layoutResult = new QDBusPendingCallWatcher(m_keyboardDBusProxy->GetLayoutDesc(data), this);
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
    QDBusPendingCallWatcher *layoutResult = new QDBusPendingCallWatcher(m_keyboardDBusProxy->GetLayoutDesc(value), this);
    connect(layoutResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onCurrentLayoutFinished);
}

void KeyboardWorker::onSearchShortcuts(const QString &searchKey)
{
    qDebug() << "onSearchShortcuts: " << searchKey;
    QDBusPendingReply<QString> reply = m_keyboardDBusProxy->SearchShortcuts(searchKey);
    QDBusPendingCallWatcher *searchResult = new QDBusPendingCallWatcher(reply, this);
    connect(searchResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onSearchFinished);
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

    const auto &layouts = m_model->kbLayout();
    Q_FOREACH(const QString &key, layouts.keys()) {
        MetaData md;
        QString title = layouts[key];
        md.setText(title);
        md.setKey(key);
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
        // ListView.section does not need this....
        // QChar ch = '\0';
        // for (int i(0); i != m_metaDatas.size(); ++i)
        // {
        //     const QChar flag = m_metaDatas[i].pinyin().at(0).toUpper();
        //     if (flag == ch)
        //         continue;
        //     ch = flag;

        //     m_letters.append(ch);
        //     m_metaDatas.insert(i, MetaData(ch, true));
        // }
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
    QDBusPendingCallWatcher *result = new QDBusPendingCallWatcher(m_keyboardDBusProxy->Query(id, type));
    connect(result, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onGetShortcutFinished);
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
    if (m_shortcutModel)
        m_shortcutModel->setCurrentInfo(info);

    m_keyboardDBusProxy->SelectKeystroke();
}

void KeyboardWorker::cleanShortcutSlef(const QString &id, const int type, const QString &shortcut)
{
    QDBusPendingCall call = m_keyboardDBusProxy->ClearShortcutKeystrokes(id, type);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

    watcher->setProperty("id", id);
    watcher->setProperty("type", type);
    watcher->setProperty("shortcut", shortcut);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onShortcutCleanFinished);
}

void KeyboardWorker::setNewCustomShortcut(const QString &id, const QString &name, const QString &command, const QString &accles)
{
    m_keyboardDBusProxy->ModifyCustomShortcut(id, name, command, accles);
}

void KeyboardWorker::onConflictShortcutCleanFinished(QDBusPendingCallWatcher *watch)
{
    if (!watch->isError()) {
        const QString &id = watch->property("id").toString();
        const int type = watch->property("type").toInt();
        const QString &shortcut = watch->property("shortcut").toString();
        const bool clean = watch->property("clean").toBool();

        if (clean) {
            cleanShortcutSlef(id, type, shortcut);
        } else {
            m_keyboardDBusProxy->AddShortcutKeystroke(id, type, shortcut);
        }
    }

    watch->deleteLater();
}

void KeyboardWorker::onShortcutCleanFinished(QDBusPendingCallWatcher *watch)
{
    if (!watch->isError()) {
        const QString &id = watch->property("id").toString();
        const int type = watch->property("type").toInt();
        const QString &shortcut = watch->property("shortcut").toString();

        m_keyboardDBusProxy->AddShortcutKeystroke(id, type, shortcut);

        if (shortcut.contains("Delete") && !shortcut.contains("KP_Delete")) {
            ShortcutInfo si;
            si.id = id;
            si.type = static_cast<uint>(type);
            si.accels = shortcut;
            si.accels = si.accels.replace("Delete", "KP_Delete");
            modifyShortcutEditAux(&si, true);
        }
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
    m_keyboardDBusProxy->setCurrentLayout(value);
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
