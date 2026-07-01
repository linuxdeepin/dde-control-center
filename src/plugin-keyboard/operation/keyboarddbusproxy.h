//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KEYBOARDDBUSPROXY_H
#define KEYBOARDDBUSPROXY_H

#include "ikeyboarddeviceproxy.h"

#include <DDBusInterface>

#include <QObject>
#include <QDBusPendingReply>

class QDBusInterface;
class QDBusMessage;

using Dtk::Core::DDBusInterface;

typedef QMap<QString, QString> KeyboardLayoutList;

class LocaleInfo
{
public:
    LocaleInfo(){}

    friend QDBusArgument &operator<<(QDBusArgument &arg, const LocaleInfo &info)
    {
        arg.beginStructure();
        arg << info.id << info.name;
        arg.endStructure();

        return arg;
    }

    friend const QDBusArgument &operator>>(const QDBusArgument &arg, LocaleInfo &info)
    {
        arg.beginStructure();
        arg >> info.id >> info.name;
        arg.endStructure();

        return arg;
    }

    friend QDataStream &operator<<(QDataStream &ds, const LocaleInfo &info)
    {
        return ds << info.id << info.name;
    }

    friend const QDataStream &operator>>(QDataStream &ds, LocaleInfo &info)
    {
        return ds >> info.id >> info.name;
    }

    bool operator ==(const LocaleInfo &info)
    {
        return id==info.id && name==info.name;
    }

public:
    QString id{""};
    QString name{""};
};

typedef QList<LocaleInfo> LocaleList;
namespace dccV25 {
class DCCDBusInterface;
}

// New API ShortcutInfo struct (dde-services)
struct ShortcutInfoNew {
    QString id;
    QString displayName;
    int category;
    QStringList hotkeys;
    QString localLanguageName;
};
Q_DECLARE_METATYPE(ShortcutInfoNew)
Q_DECLARE_METATYPE(QList<ShortcutInfoNew>)

inline QDBusArgument &operator<<(QDBusArgument &argument, const ShortcutInfoNew &info) {
    argument.beginStructure();
    argument << info.id << info.displayName << info.category
             << info.hotkeys << info.localLanguageName;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, ShortcutInfoNew &info) {
    argument.beginStructure();
    argument >> info.id >> info.displayName >> info.category
             >> info.hotkeys >> info.localLanguageName;
    argument.endStructure();
    return argument;
}

class KeyboardDBusProxy : public QObject, public DCC_NAMESPACE::IKeyboardDeviceProxy
{
    Q_OBJECT
public:
    explicit KeyboardDBusProxy(QObject *parent = nullptr);
    void active() override {}
    void deactive() override {}

    //Keyboard
    Q_PROPERTY(bool CapslockToggle READ capslockToggle WRITE setCapslockToggle NOTIFY CapslockToggleChanged)
    bool capslockToggle();
    void setCapslockToggle(bool value);

    Q_PROPERTY(QString CurrentLayout READ currentLayout WRITE setCurrentLayout NOTIFY CurrentLayoutChanged)
    QString currentLayout();
    void setCurrentLayout(const QString &value);

    Q_PROPERTY(int CursorBlink READ cursorBlink WRITE setCursorBlink NOTIFY CursorBlinkChanged)
    int cursorBlink();
    void setCursorBlink(int value);

    Q_PROPERTY(int LayoutScope READ layoutScope WRITE setLayoutScope NOTIFY LayoutScopeChanged)
    int layoutScope();
    void setLayoutScope(int value);

    Q_PROPERTY(uint RepeatDelay READ repeatDelay WRITE setRepeatDelay NOTIFY RepeatDelayChanged)
    uint repeatDelay() const override;
    void setRepeatDelay(uint value);

    Q_PROPERTY(bool RepeatEnabled READ repeatEnabled WRITE setRepeatEnabled NOTIFY RepeatEnabledChanged)
    bool repeatEnabled();
    void setRepeatEnabled(bool value);

    Q_PROPERTY(uint RepeatInterval READ repeatInterval WRITE setRepeatInterval NOTIFY RepeatIntervalChanged)
    uint repeatInterval() const override;
    void setRepeatInterval(uint value);

    Q_PROPERTY(QStringList UserLayoutList READ userLayoutList NOTIFY UserLayoutListChanged)
    QStringList userLayoutList();

    Q_PROPERTY(QStringList UserOptionList READ userOptionList NOTIFY UserOptionListChanged)
    QStringList userOptionList();

    //LangSelector
    Q_PROPERTY(QString CurrentLocale READ currentLocale NOTIFY CurrentLocaleChanged)
    QString currentLocale();

    Q_PROPERTY(int LocaleState READ localeState NOTIFY LocaleStateChanged)
    int localeState();

    Q_PROPERTY(QStringList Locales READ locales NOTIFY LocalesChanged)
    QStringList locales();

    //Keybinding
    Q_PROPERTY(int NumLockState READ numLockState NOTIFY NumLockStateChanged)
    int numLockState() const override;
    void setNumLockState(int value) override;

    Q_PROPERTY(uint ShortcutSwitchLayout READ shortcutSwitchLayout WRITE setShortcutSwitchLayout NOTIFY ShortcutSwitchLayoutChanged)
    uint shortcutSwitchLayout();
    void setShortcutSwitchLayout(uint value);

    bool langSelectorIsValid();
    void langSelectorStartServiceProcess();

signals:
    // Keyboard property
    void CapslockToggleChanged(bool  value) const;
    void CurrentLayoutChanged(const QString & value) const;
    void CursorBlinkChanged(int  value) const;
    void LayoutScopeChanged(int  value) const;
    void RepeatDelayChanged(uint  value) const;
    void RepeatEnabledChanged(bool  value) const;
    void RepeatIntervalChanged(uint  value) const;
    void UserLayoutListChanged(const QStringList & value) const;
    void UserOptionListChanged(const QStringList & value) const;

    // LangSelector property
    void CurrentLocaleChanged(const QString & value) const;
    void LocaleStateChanged(int  value) const;
    void LocalesChanged(const QStringList & value) const;

    // Keybinding property
    void NumLockStateChanged(int  value) const;
    void ShortcutSwitchLayoutChanged(uint  value) const;
    // Keybinding
    void Added(const QString &in0, int in1);
    void Changed(const QString &in0, int in1);
    void Deleted(const QString &in0, int in1);
    void KeyEvent(bool in0, const QString &in1);
    // Wayland new-API bridge: full shortcut list converted to old JSON format.
    void AllShortcutsReady(const QString &json);
    // Wayland: result of a GetShortcut query, already converted to old JSON.
    void shortcutQueried(const QString &json);
    // Wayland: result of SearchShortcuts converted to old JSON array.
    void searchShortcutsReady(const QString &json);
    // Wayland: server-originated removal (ShortcutRemoved). Dedicated signal —
    // never reuse Deleted, whose X11 auto-wiring → KeyboardWorker::removed is a
    // dead-end relay. Carries only the id (the new-API signal has no type).
    void shortcutRemovedById(const QString &id);

    //wm
    void compositingEnabledChanged(bool enabled);

    void langSelectorServiceStartFinished(const quint32 ret) const;

public slots:
    // Keybinding
    QDBusPendingReply<>  KeybindingReset();
    QDBusPendingReply<QString> ListAllShortcuts();
    QString LookupConflictingShortcut(const QString &in0);
    QDBusPendingReply<ShortcutInfoNew> LookupConflictShortcut(const QString &in0);
    QDBusPendingReply<> ClearShortcutKeystrokes(const QString &in0, int in1);
    QDBusPendingReply<> AddShortcutKeystroke(const QString &in0, int in1, const QString &in2);
    // Wayland: direct ModifyHotkeys (returns bool success), so the caller
    // can detect commit failures and roll back the UI.
    QDBusPendingReply<bool> callModifyHotkeys(const QString &id, const QStringList &hotkeys);
    // Wayland: replace hotkey — remove newHotkey from conflictId, add to targetId.
    QDBusPendingReply<bool> callReplaceHotkey(const QString &targetId, const QString &newHotkey, const QString &conflictId);
    QDBusPendingReply<> AddCustomShortcut(const QString &in0, const QString &in1, const QString &in2);
    QDBusPendingReply<> ModifyCustomShortcut(const QString &in0, const QString &in1, const QString &in2, const QString &in3);
    QDBusPendingReply<> DeleteCustomShortcut(const QString &in0);
    QDBusPendingReply<> GrabScreen();
    void SetNumLockState(int in0);
    QDBusPendingReply<QString> GetShortcut(const QString &in0, int in1);
    QDBusPendingReply<QString> SearchShortcuts(const QString &in0);
    QDBusPendingReply<QString> Query(const QString &in0, int in1);
    void SelectKeystroke();

    //KeyBoard
    QDBusPendingReply<KeyboardLayoutList> LayoutList();
    QDBusPendingReply<KeyboardLayoutList> AllLayoutList();
    void AddUserLayout(const QString &in0);
    void DeleteUserLayout(const QString &in0);
    QDBusPendingReply<QString> GetLayoutDesc(const QString &in0);

    QDBusPendingReply<> AddLocale(const QString &in0);
    QDBusPendingReply<> DeleteLocale(const QString &in0);
    QDBusPendingReply<> SetLocale(const QString &in0);
    QDBusPendingReply<LocaleList> GetLocaleList();

private slots:
    void onLangSelectorStartServiceProcessFinished(QDBusPendingCallWatcher *w);
    // New API -> Old API adapters (Wayland)
    void onListAllShortcutsNewFinished(QDBusPendingCallWatcher *w);
    void onGetShortcutNewFinished(QDBusPendingCallWatcher *w);
    void onSearchShortcutsNewFinished(QDBusPendingCallWatcher *w);
    // Bridge: new dde-services D-Bus signals (ShortcutChanged/ShortcutRemoved)
    // are named differently from the legacy Changed/Deleted, so auto-wiring
    // by QDBusAbstractInterface misses them. Subscribe explicitly and forward.
    void onNewShortcutChanged(const QString &id, const ShortcutInfoNew &info);
    void onNewShortcutRemoved(const QString &id);

private:
    void init();

public:
    bool isWayland() const;

private:
    // Convert new ShortcutInfo list to old JSON format
    QString convertShortcutListToJson(const QList<ShortcutInfoNew> &list) const;
    QString convertShortcutToJson(const ShortcutInfoNew &info) const;

    DDBusInterface *m_dBusLangSelectorInter;
    DDBusInterface *m_dBusKeyboardInter;
    DDBusInterface *m_dBusKeybingdingInter;
    DDBusInterface *m_dBusWMInter;
    const bool m_isWayland;
};

Q_DECLARE_METATYPE(KeyboardLayoutList)
Q_DECLARE_METATYPE(LocaleInfo)
Q_DECLARE_METATYPE(LocaleList)

#endif // KeyboardDBusProxy_H
