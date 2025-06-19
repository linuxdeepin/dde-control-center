//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "keyboarddbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusMetaType>
#include <QDebug>

const static QString LangSelectorService = "org.deepin.dde.LangSelector1";
const static QString LangSelectorPath = "/org/deepin/dde/LangSelector1";
const static QString LangSelectorInterface = "org.deepin.dde.LangSelector1";

const static QString KeyboardService = "org.deepin.dde.InputDevices1";
const static QString KeyboardPath = "/org/deepin/dde/InputDevice1/Keyboard";
const static QString KeyboardInterface = "org.deepin.dde.InputDevice1.Keyboard";

const static QString KeybingdingService = "org.deepin.dde.Keybinding1";
const static QString KeybingdingPath = "/org/deepin/dde/Keybinding1";
const static QString KeybingdingInterface = "org.deepin.dde.Keybinding1";

const static QString WMService = "com.deepin.wm";
const static QString WMPath = "/com/deepin/wm";
const static QString WMInterface = "com.deepin.wm";

KeyboardDBusProxy::KeyboardDBusProxy(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<KeyboardLayoutList>("KeyboardLayoutList");
    qDBusRegisterMetaType<KeyboardLayoutList>();

    qRegisterMetaType<LocaleInfo>("LocaleInfo");
    qDBusRegisterMetaType<LocaleInfo>();

    qRegisterMetaType<LocaleList>("LocaleList");
    qDBusRegisterMetaType<LocaleList>();

    init();
}

void KeyboardDBusProxy::init()
{
    m_dBusLangSelectorInter = new DDBusInterface(LangSelectorService, LangSelectorPath, LangSelectorInterface, QDBusConnection::sessionBus(), this);
    m_dBusKeyboardInter = new DDBusInterface(KeyboardService, KeyboardPath, KeyboardInterface, QDBusConnection::sessionBus(), this);
    m_dBusKeybingdingInter = new DDBusInterface(KeybingdingService, KeybingdingPath, KeybingdingInterface, QDBusConnection::sessionBus(), this);
    m_dBusWMInter = new DDBusInterface(WMService, WMPath, WMInterface, QDBusConnection::sessionBus(), this);

    QDBusConnection::sessionBus().connect(WMService, WMPath, WMInterface, "wmCompositingEnabledChanged", this, SIGNAL(compositingEnabledChanged(bool)));
}

void KeyboardDBusProxy::langSelectorStartServiceProcess()
{
    if (m_dBusLangSelectorInter->isValid())
    {
        qWarning() << "Service" << LangSelectorService << "is already started.";
        return;
    }

    QDBusInterface freedesktopInter = QDBusInterface("org.freedesktop.DBus", "/", "org.freedesktop.DBus", QDBusConnection::systemBus(), this);
    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.DBus", "/", "org.freedesktop.DBus", QStringLiteral("StartServiceByName"));
    msg << LangSelectorService << quint32(0);
    QDBusPendingReply<quint32> async = freedesktopInter.connection().asyncCall(msg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, &KeyboardDBusProxy::onLangSelectorStartServiceProcessFinished);
}

void KeyboardDBusProxy::onLangSelectorStartServiceProcessFinished(QDBusPendingCallWatcher *w)
{

    QDBusPendingReply<quint32> reply = *w;
    Q_EMIT langSelectorServiceStartFinished(reply.value());
    w->deleteLater();
}

//Keyboard
bool KeyboardDBusProxy::capslockToggle()
{
    return qvariant_cast<bool>(m_dBusKeyboardInter->property("CapslockToggle"));
}

void KeyboardDBusProxy::setCapslockToggle(bool value)
{
    m_dBusKeyboardInter->setProperty("CapslockToggle", QVariant::fromValue(value));
}

QString KeyboardDBusProxy::currentLayout()
{
    return qvariant_cast<QString>(m_dBusKeyboardInter->property("CurrentLayout"));
}

void KeyboardDBusProxy::setCurrentLayout(const QString &value)
{
    m_dBusKeyboardInter->setProperty("CurrentLayout", QVariant::fromValue(value));
}

int KeyboardDBusProxy::cursorBlink()
{
    return qvariant_cast<int>(m_dBusKeyboardInter->property("CursorBlink"));
}

void KeyboardDBusProxy::setCursorBlink(int value)
{
   m_dBusKeyboardInter->setProperty("CursorBlink", QVariant::fromValue(value));
}

int KeyboardDBusProxy::layoutScope()
{
    return qvariant_cast<int>(m_dBusKeyboardInter->property("LayoutScope"));
}

void KeyboardDBusProxy::setLayoutScope(int value)
{
    m_dBusKeyboardInter->setProperty("LayoutScope", QVariant::fromValue(value));
}


uint KeyboardDBusProxy::repeatDelay()
{
    return qvariant_cast<uint>(m_dBusKeyboardInter->property("RepeatDelay"));
}

void KeyboardDBusProxy::setRepeatDelay(uint value)
{
    m_dBusKeyboardInter->setProperty("RepeatDelay", QVariant::fromValue(value));
}


bool KeyboardDBusProxy::repeatEnabled()
{
    return qvariant_cast<bool>(m_dBusKeyboardInter->property("RepeatEnabled"));
}

void KeyboardDBusProxy::setRepeatEnabled(bool value)
{
    m_dBusKeyboardInter->setProperty("RepeatEnabled", QVariant::fromValue(value));
}


uint KeyboardDBusProxy::repeatInterval()
{
    return qvariant_cast<uint>(m_dBusKeyboardInter->property("RepeatInterval"));
}

void KeyboardDBusProxy::setRepeatInterval(uint value)
{
    m_dBusKeyboardInter->setProperty("RepeatInterval", QVariant::fromValue(value));
}


QStringList KeyboardDBusProxy::userLayoutList()
{
    return qvariant_cast<QStringList>(m_dBusKeyboardInter->property("UserLayoutList"));
}


QStringList KeyboardDBusProxy::userOptionList()
{
    return qvariant_cast<QStringList>(m_dBusKeyboardInter->property("UserOptionList"));
}

//LangSelector
QString KeyboardDBusProxy::currentLocale()
{
    return qvariant_cast<QString>(m_dBusLangSelectorInter->property("CurrentLocale"));
}

int KeyboardDBusProxy::localeState()
{
    return qvariant_cast<int>(m_dBusLangSelectorInter->property("LocaleState"));
}

QStringList KeyboardDBusProxy::locales()
{
    return qvariant_cast<QStringList>(m_dBusLangSelectorInter->property("Locales"));
}

//Keybinding
int KeyboardDBusProxy::numLockState()
{
    return qvariant_cast<int>(m_dBusKeybingdingInter->property("NumLockState"));
}

uint KeyboardDBusProxy::shortcutSwitchLayout()
{
    return qvariant_cast<uint>(m_dBusKeybingdingInter->property("ShortcutSwitchLayout"));
}

void KeyboardDBusProxy::setShortcutSwitchLayout(uint value)
{
    m_dBusKeybingdingInter->setProperty("ShortcutSwitchLayout", QVariant::fromValue(value));
}

bool KeyboardDBusProxy::langSelectorIsValid()
{
    return m_dBusLangSelectorInter->isValid();
}

QDBusPendingReply<> KeyboardDBusProxy::KeybindingReset()
{
    QList<QVariant> argumentList;
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("Reset"), argumentList);
}

QDBusPendingReply<QString> KeyboardDBusProxy::ListAllShortcuts()
{
    QList<QVariant> argumentList;
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("ListAllShortcuts"), argumentList);
}

QString KeyboardDBusProxy::LookupConflictingShortcut(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return QDBusPendingReply<QString>(m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("LookupConflictingShortcut"), argumentList));
}

QDBusPendingReply<> KeyboardDBusProxy::ClearShortcutKeystrokes(const QString &in0, int in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("ClearShortcutKeystrokes"), argumentList);
}

QDBusPendingReply<> KeyboardDBusProxy::AddShortcutKeystroke(const QString &in0, int in1, const QString &in2)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2);
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("AddShortcutKeystroke"), argumentList);
}

QDBusPendingReply<> KeyboardDBusProxy::AddCustomShortcut(const QString &in0, const QString &in1, const QString &in2)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2);
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("AddCustomShortcut"), argumentList);
}

QDBusPendingReply<> KeyboardDBusProxy::ModifyCustomShortcut(const QString &in0, const QString &in1, const QString &in2, const QString &in3)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2) << QVariant::fromValue(in3);
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("ModifyCustomShortcut"), argumentList);
}

QDBusPendingReply<> KeyboardDBusProxy::DeleteCustomShortcut(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("DeleteCustomShortcut"), argumentList);
}

QDBusPendingReply<> KeyboardDBusProxy::GrabScreen()
{
    QList<QVariant> argumentList;
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("GrabScreen"), argumentList);
}

void KeyboardDBusProxy::SetNumLockState(int in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("SetNumLockState"), argumentList);
}

QDBusPendingReply<QString> KeyboardDBusProxy::GetShortcut(const QString &in0, int in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("GetShortcut"), argumentList);
}

QDBusPendingReply<QString> KeyboardDBusProxy::SearchShortcuts(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("SearchShortcuts"), argumentList);
}

QDBusPendingReply<QString> KeyboardDBusProxy::Query(const QString &in0, int in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("Query"), argumentList);
}

void KeyboardDBusProxy::SelectKeystroke()
{
    QList<QVariant> argumentList;
    m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("SelectKeystroke"), argumentList);
}

//keyBoard
QDBusPendingReply<KeyboardLayoutList> KeyboardDBusProxy::LayoutList()
{
    QList<QVariant> argumentList;
    return m_dBusKeyboardInter->asyncCallWithArgumentList(QStringLiteral("LayoutList"), argumentList);
}

QDBusPendingReply<KeyboardLayoutList> KeyboardDBusProxy::AllLayoutList()
{
    QList<QVariant> argumentList;
    return m_dBusKeyboardInter->asyncCallWithArgumentList(QStringLiteral("AllLayoutList"), argumentList);
}

void KeyboardDBusProxy::AddUserLayout(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_dBusKeyboardInter->asyncCallWithArgumentList(QStringLiteral("AddUserLayout"), argumentList);
}

void KeyboardDBusProxy::DeleteUserLayout(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_dBusKeyboardInter->asyncCallWithArgumentList(QStringLiteral("DeleteUserLayout"), argumentList);
}

QDBusPendingReply<QString> KeyboardDBusProxy::GetLayoutDesc(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusKeyboardInter->asyncCallWithArgumentList(QStringLiteral("GetLayoutDesc"), argumentList);
}

QDBusPendingReply<> KeyboardDBusProxy::AddLocale(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusLangSelectorInter->asyncCallWithArgumentList(QStringLiteral("AddLocale"), argumentList);
}

QDBusPendingReply<> KeyboardDBusProxy::DeleteLocale(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusLangSelectorInter->asyncCallWithArgumentList(QStringLiteral("DeleteLocale"), argumentList);
}

QDBusPendingReply<> KeyboardDBusProxy::SetLocale(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusLangSelectorInter->asyncCallWithArgumentList(QStringLiteral("SetLocale"), argumentList);
}

QDBusPendingReply<LocaleList> KeyboardDBusProxy::GetLocaleList()
{
    QList<QVariant> argumentList;
    return m_dBusLangSelectorInter->asyncCallWithArgumentList(QStringLiteral("GetLocaleList"), argumentList);
}
