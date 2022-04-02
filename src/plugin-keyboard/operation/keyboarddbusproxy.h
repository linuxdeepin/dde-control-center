/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#ifndef KEYBOARDDBUSPROXY_H
#define KEYBOARDDBUSPROXY_H

#include <QObject>
#include <QDBusPendingReply>

class QDBusInterface;
class QDBusMessage;

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

class KeyboardDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardDBusProxy(QObject *parent = nullptr);

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
    uint repeatDelay();
    void setRepeatDelay(uint value);

    Q_PROPERTY(bool RepeatEnabled READ repeatEnabled WRITE setRepeatEnabled NOTIFY RepeatEnabledChanged)
    bool repeatEnabled();
    void setRepeatEnabled(bool value);

    Q_PROPERTY(uint RepeatInterval READ repeatInterval WRITE setRepeatInterval NOTIFY RepeatIntervalChanged)
    uint repeatInterval();
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
    int numLockState();

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

    //wm
    void compositingEnabledChanged(bool enabled);

    void langSelectorServiceStartFinished(const quint32 ret) const;

public slots:
    // Keybinding
    QDBusPendingReply<>  KeybindingReset();
    QDBusPendingReply<QString> ListAllShortcuts();
    QString LookupConflictingShortcut(const QString &in0);
    QDBusPendingReply<> ClearShortcutKeystrokes(const QString &in0, int in1);
    QDBusPendingReply<> AddShortcutKeystroke(const QString &in0, int in1, const QString &in2);
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
    void AddUserLayout(const QString &in0);
    void DeleteUserLayout(const QString &in0);
    QDBusPendingReply<QString> GetLayoutDesc(const QString &in0);

    QDBusPendingReply<> AddLocale(const QString &in0);
    QDBusPendingReply<> DeleteLocale(const QString &in0);
    QDBusPendingReply<> SetLocale(const QString &in0);
    QDBusPendingReply<LocaleList> GetLocaleList();

private slots:
    void onPropertiesChanged(const QDBusMessage &message);
    void onLangSelectorStartServiceProcessFinished(QDBusPendingCallWatcher *w);
private:
    void init();

private:
    QDBusInterface *m_dBusLangSelectorInter;
    QDBusInterface *m_dBusKeyboardInter;
    QDBusInterface *m_dBusKeybingdingInter;
    QDBusInterface *m_dBusWMInter;
};

Q_DECLARE_METATYPE(KeyboardLayoutList)
Q_DECLARE_METATYPE(LocaleInfo)
Q_DECLARE_METATYPE(LocaleList)

#endif // KeyboardDBusProxy_H
