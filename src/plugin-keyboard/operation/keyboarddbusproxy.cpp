//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "keyboarddbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>
#include <QDBusMetaType>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

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
    , m_isWayland((qgetenv("XDG_SESSION_TYPE").toLower() == QLatin1String("wayland"))
                  || !qgetenv("WAYLAND_DISPLAY").isEmpty())
{
    qRegisterMetaType<KeyboardLayoutList>("KeyboardLayoutList");
    qDBusRegisterMetaType<KeyboardLayoutList>();

    qRegisterMetaType<LocaleInfo>("LocaleInfo");
    qDBusRegisterMetaType<LocaleInfo>();

    qRegisterMetaType<LocaleList>("LocaleList");
    qDBusRegisterMetaType<LocaleList>();

    // Register new API types
    qRegisterMetaType<ShortcutInfoNew>("ShortcutInfoNew");
    qDBusRegisterMetaType<ShortcutInfoNew>();
    qRegisterMetaType<QList<ShortcutInfoNew>>("QList<ShortcutInfoNew>");
    qDBusRegisterMetaType<QList<ShortcutInfoNew>>();

    qRegisterMetaType<CategoryInfoNew>("CategoryInfoNew");
    qDBusRegisterMetaType<CategoryInfoNew>();
    qRegisterMetaType<QList<CategoryInfoNew>>("QList<CategoryInfoNew>");
    qDBusRegisterMetaType<QList<CategoryInfoNew>>();

    init();
}

bool KeyboardDBusProxy::isWayland() const
{
    return m_isWayland;
}

// Convert a single ShortcutInfo to old-API JSON object
static QJsonObject shortcutInfoToJsonObject(const ShortcutInfoNew &info)
{
    QJsonObject obj;
    obj["Id"] = info.id;
    // Prefer translated name over raw displayName; fall back if empty
    obj["Name"] = info.localLanguageName.isEmpty() ? info.displayName : info.localLanguageName;
    // dde-services already emits hotkeys in X11/XKB form ("<Control><Alt>T",
    // "XF86AudioMute"), so we just forward the first one to Accels.
    obj["Accels"] = QJsonArray{info.hotkeys.isEmpty() ? QString() : info.hotkeys.first()};
    // Type is now driven by the explicit isCustom flag from dde-services
    // (was derived from category==3). 0=System, 1=Custom.
    obj["Type"] = info.isCustom ? 1 : 0;
    obj["Exec"] = QString();
    // Section carries the stable logical category key (for grouping/ordering
    // and Custom detection); SectionName carries the already-resolved display
    // text for direct rendering — dcc no longer translates categories.
    obj["Section"] = info.category;
    obj["SectionName"] = info.localLanguageCategory;
    return obj;
}

QString KeyboardDBusProxy::convertShortcutListToJson(const QList<ShortcutInfoNew> &list) const
{
    QJsonArray array;
    for (const auto &info : list) {
        array.append(shortcutInfoToJsonObject(info));
    }
    return QJsonDocument(array).toJson(QJsonDocument::Compact);
}

QString KeyboardDBusProxy::convertShortcutToJson(const ShortcutInfoNew &info) const
{
    return QJsonDocument(shortcutInfoToJsonObject(info)).toJson(QJsonDocument::Compact);
}

void KeyboardDBusProxy::requestShortcutCommand(const ShortcutInfoNew &info)
{
    if (!isWayland() || !info.isCustom || info.id.isEmpty())
        return;

    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(info.id);
    QDBusPendingCall call = m_dBusKeybingdingInter->asyncCallWithArgumentList(
        QStringLiteral("GetShortcutCommand"), argumentList);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    // Stash the full JSON object built from info; the finished handler only
    // needs to overlay the Exec field from the reply. Avoids re-listing the
    // field names here and reconstructing the object key-by-key on reply.
    watcher->setProperty("shortcutJson", shortcutInfoToJsonObject(info));
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &KeyboardDBusProxy::onGetShortcutCommandFinished);
}

void KeyboardDBusProxy::init()
{
    m_dBusLangSelectorInter = new DDBusInterface(LangSelectorService, LangSelectorPath, LangSelectorInterface, QDBusConnection::sessionBus(), this);
    m_dBusKeyboardInter = new DDBusInterface(KeyboardService, KeyboardPath, KeyboardInterface, QDBusConnection::sessionBus(), this);
    m_dBusKeybingdingInter = new DDBusInterface(KeybingdingService, KeybingdingPath, KeybingdingInterface, QDBusConnection::sessionBus(), this);
    m_dBusWMInter = new DDBusInterface(WMService, WMPath, WMInterface, QDBusConnection::sessionBus(), this);

    QDBusConnection::sessionBus().connect(WMService, WMPath, WMInterface, "wmCompositingEnabledChanged", this, SIGNAL(compositingEnabledChanged(bool)));

    // dde-services emits ShortcutChanged / ShortcutRemoved on the new API,
    // which don't match the legacy Changed/Deleted names auto-wired by
    // QDBusAbstractInterface. Subscribe by full signature so the model
    // refreshes after server-side updates (e.g. Reset).
    if (m_isWayland) {
        QDBusConnection::sessionBus().connect(
            KeybingdingService, KeybingdingPath, KeybingdingInterface,
            "ShortcutChanged", this,
            SLOT(onNewShortcutChanged(QString, ShortcutInfoNew)));
        QDBusConnection::sessionBus().connect(
            KeybingdingService, KeybingdingPath, KeybingdingInterface,
            "ShortcutRemoved", this,
            SLOT(onNewShortcutRemoved(QString)));
    }
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


uint KeyboardDBusProxy::repeatDelay() const
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


uint KeyboardDBusProxy::repeatInterval() const
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
int KeyboardDBusProxy::numLockState() const
{
    return qvariant_cast<int>(m_dBusKeybingdingInter->property("NumLockState"));
}

void KeyboardDBusProxy::setNumLockState(int value)
{
    SetNumLockState(value != 0);
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

// ---- Keybinding methods ----
// In Wayland mode, call new dde-services API and convert to old JSON format.
// In X11 mode, keep existing old API calls.

QDBusPendingReply<> KeyboardDBusProxy::KeybindingReset()
{
    QList<QVariant> argumentList;
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("Reset"), argumentList);
}

// Wayland: called when new API ListAllShortcuts() async reply arrives
void KeyboardDBusProxy::onListAllShortcutsNewFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QList<ShortcutInfoNew>> reply = *w;
    if (!reply.isError()) {
        const QList<ShortcutInfoNew> shortcuts = reply.value();
        QString json = convertShortcutListToJson(shortcuts);
        Q_EMIT AllShortcutsReady(json);
        for (const ShortcutInfoNew &info : shortcuts)
            requestShortcutCommand(info);
    } else {
        qWarning() << "Wayland ListAllShortcuts failed:" << reply.error();
    }
    w->deleteLater();
}

QDBusPendingReply<QString> KeyboardDBusProxy::ListAllShortcuts()
{
    QList<QVariant> argumentList;
    if (isWayland()) {
        // New API: ListAllShortcuts() → QList<ShortcutInfoNew>
        // We fire-and-forget, result arrives in onListAllShortcutsNewFinished
        QDBusPendingCall call = m_dBusKeybingdingInter->asyncCallWithArgumentList(
            QStringLiteral("ListAllShortcuts"), argumentList);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished,
                this, &KeyboardDBusProxy::onListAllShortcutsNewFinished);
        // Return dummy — actual data comes via Added signal
        return QDBusPendingReply<QString>();
    }
    // Old API
    return QDBusPendingReply<QString>(
        m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("ListAllShortcuts"), argumentList));
}

// Wayland: fetch category metadata (key/displayName/order/isCustom) so the
// model can group, order, and identify the Custom group without hardcoding
// any category strings.
QDBusPendingReply<> KeyboardDBusProxy::ListCategories()
{
    QList<QVariant> argumentList;
    if (isWayland()) {
        QDBusPendingCall call = m_dBusKeybingdingInter->asyncCallWithArgumentList(
            QStringLiteral("ListCategories"), argumentList);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished,
                this, &KeyboardDBusProxy::onListCategoriesFinished);
    }
    return QDBusPendingReply<>();
}

void KeyboardDBusProxy::onListCategoriesFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QList<CategoryInfoNew>> reply = *w;
    if (!reply.isError()) {
        Q_EMIT categoriesReady(reply.value());
    } else {
        qWarning() << "Wayland ListCategories failed:" << reply.error();
    }
    w->deleteLater();
}

// Wayland: called when new API GetShortcut() async reply arrives.
// The result is delivered via shortcutQueried so the downstream
// model-update path never re-enters onShortcutChanged → Query → ...
void KeyboardDBusProxy::onGetShortcutNewFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<ShortcutInfoNew> reply = *w;
    if (!reply.isError()) {
        const ShortcutInfoNew info = reply.value();
        QString json = convertShortcutToJson(info);
        Q_EMIT shortcutQueried(json);
        requestShortcutCommand(info);
    }
    w->deleteLater();
}

// Server emits ShortcutChanged after any in-memory + dconfig commit (Modify,
// Reset, Disable, external dconfig writes). Forward via shortcutQueried so
// the model updates without an extra GetShortcut round-trip — the signal
// payload already carries the full info.
void KeyboardDBusProxy::onNewShortcutChanged(const QString &id, const ShortcutInfoNew &info)
{
    Q_UNUSED(id);
    Q_EMIT shortcutQueried(convertShortcutToJson(info));
    requestShortcutCommand(info);
}

void KeyboardDBusProxy::onGetShortcutCommandFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;
    if (!reply.isError()) {
        QJsonObject obj = w->property("shortcutJson").toJsonObject();
        obj["Exec"] = reply.value();
        Q_EMIT shortcutQueried(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    }
    w->deleteLater();
}

void KeyboardDBusProxy::onNewShortcutRemoved(const QString &id)
{
    // Forward to a dedicated signal that ShortcutModel::removeShortcutById
    // consumes. Do NOT reuse Deleted(id, type): its only connection
    // (KeyboardWorker::removed) is an unused forward, so the row would never
    // leave the UI on a server-side removal (e.g. Reset). The new-API signal
    // carries no type, which is fine — removal looks up by id across all lists.
    Q_EMIT shortcutRemovedById(id);
}

QDBusPendingReply<QString> KeyboardDBusProxy::GetShortcut(const QString &in0, int in1)
{
    Q_UNUSED(in1);
    QList<QVariant> argumentList;
    if (isWayland()) {
        // New API: GetShortcut(id) → ShortcutInfoNew
        argumentList << QVariant::fromValue(in0);
        QDBusPendingCall call = m_dBusKeybingdingInter->asyncCallWithArgumentList(
            QStringLiteral("GetShortcut"), argumentList);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        watcher->setProperty("shortcutId", in0);
        connect(watcher, &QDBusPendingCallWatcher::finished,
                this, &KeyboardDBusProxy::onGetShortcutNewFinished);
        return QDBusPendingReply<QString>();
    }
    // Old API
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return QDBusPendingReply<QString>(
        m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("GetShortcut"), argumentList));
}

QString KeyboardDBusProxy::LookupConflictingShortcut(const QString &in0)
{
    QList<QVariant> argumentList;
    if (isWayland()) {
        qWarning() << "Wayland LookupConflictingShortcut is asynchronous; use LookupConflictShortcut instead";
        return QString();
    }
    // Old API
    argumentList << QVariant::fromValue(in0);
    return QDBusPendingReply<QString>(
        m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("LookupConflictingShortcut"), argumentList));
}

QDBusPendingReply<ShortcutInfoNew> KeyboardDBusProxy::LookupConflictShortcut(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return QDBusPendingReply<ShortcutInfoNew>(
        m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("LookupConflictShortcut"), argumentList));
}

QDBusPendingReply<> KeyboardDBusProxy::ClearShortcutKeystrokes(const QString &in0, int in1)
{
    QList<QVariant> argumentList;
    if (isWayland()) {
        // New API: Disable(id)
        argumentList << QVariant::fromValue(in0);
        return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("Disable"), argumentList);
    }
    // Old API
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("ClearShortcutKeystrokes"), argumentList);
}

QDBusPendingReply<bool> KeyboardDBusProxy::callModifyHotkeys(const QString &id, const QStringList &hotkeys)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(id) << QVariant::fromValue(hotkeys);
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("ModifyHotkeys"), argumentList);
}

QDBusPendingReply<bool> KeyboardDBusProxy::callReplaceHotkey(const QString &targetId, const QString &newHotkey, const QString &conflictId)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(targetId) << QVariant::fromValue(newHotkey) << QVariant::fromValue(conflictId);
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("ReplaceHotkey"), argumentList);
}

QDBusPendingReply<> KeyboardDBusProxy::AddShortcutKeystroke(const QString &in0, int in1, const QString &in2)
{
    QList<QVariant> argumentList;
    if (isWayland()) {
        // New API: ModifyHotkeys(id, [keystroke])
        argumentList << QVariant::fromValue(in0) << QVariant::fromValue(QStringList{in2});
        return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("ModifyHotkeys"), argumentList);
    }
    // Old API
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2);
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("AddShortcutKeystroke"), argumentList);
}

QDBusPendingReply<QString> KeyboardDBusProxy::AddCustomShortcut(const QString &in0, const QString &in1, const QString &in2)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2);
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("AddCustomShortcut"), argumentList);
}

QDBusPendingReply<bool> KeyboardDBusProxy::ModifyCustomShortcut(const QString &in0, const QString &in1, const QString &in2, const QString &in3)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2) << QVariant::fromValue(in3);
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("ModifyCustomShortcut"), argumentList);
}

QDBusPendingReply<bool> KeyboardDBusProxy::DeleteCustomShortcut(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("DeleteCustomShortcut"), argumentList);
}

QDBusPendingReply<> KeyboardDBusProxy::GrabScreen()
{
    QList<QVariant> argumentList;
    if (isWayland()) {
        // Not supported in new API, return empty
        qWarning() << "Wayland GrabScreen not supported";
        return QDBusPendingReply<>();
    }
    return m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("GrabScreen"), argumentList);
}

void KeyboardDBusProxy::SetNumLockState(int in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("SetNumLockState"), argumentList);
}

void KeyboardDBusProxy::onSearchShortcutsNewFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QList<ShortcutInfoNew>> reply = *w;
    if (!reply.isError()) {
        const QList<ShortcutInfoNew> shortcuts = reply.value();
        QString json = convertShortcutListToJson(shortcuts);
        Q_EMIT searchShortcutsReady(json);
        for (const ShortcutInfoNew &info : shortcuts)
            requestShortcutCommand(info);
    }
    w->deleteLater();
}

QDBusPendingReply<QString> KeyboardDBusProxy::SearchShortcuts(const QString &in0)
{
    QList<QVariant> argumentList;
    if (isWayland()) {
        // New API: SearchShortcuts(keyword) → QList<ShortcutInfoNew>
        argumentList << QVariant::fromValue(in0);
        QDBusPendingCall call = m_dBusKeybingdingInter->asyncCallWithArgumentList(
            QStringLiteral("SearchShortcuts"), argumentList);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished,
                this, &KeyboardDBusProxy::onSearchShortcutsNewFinished);
        return QDBusPendingReply<QString>();
    }
    // Old API
    argumentList << QVariant::fromValue(in0);
    return QDBusPendingReply<QString>(
        m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("SearchShortcuts"), argumentList));
}

QDBusPendingReply<QString> KeyboardDBusProxy::Query(const QString &in0, int in1)
{
    QList<QVariant> argumentList;
    if (isWayland()) {
        // New API: GetShortcut(id) (Query was same as GetShortcut in old API)
        argumentList << QVariant::fromValue(in0);
        QDBusPendingCall call = m_dBusKeybingdingInter->asyncCallWithArgumentList(
            QStringLiteral("GetShortcut"), argumentList);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        watcher->setProperty("shortcutId", in0);
        connect(watcher, &QDBusPendingCallWatcher::finished,
                this, &KeyboardDBusProxy::onGetShortcutNewFinished);
        return QDBusPendingReply<QString>();
    }
    // Old API
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return QDBusPendingReply<QString>(
        m_dBusKeybingdingInter->asyncCallWithArgumentList(QStringLiteral("Query"), argumentList));
}

void KeyboardDBusProxy::SelectKeystroke()
{
    if (isWayland()) {
        // Not supported — control center handles key capture via Qt keyPressEvent
        qDebug() << "[Wayland] SelectKeystroke handled by control center natively";
        return;
    }
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
