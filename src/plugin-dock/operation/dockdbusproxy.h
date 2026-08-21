//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DOCKDBUSPROXY_H
#define DOCKDBUSPROXY_H

#include <QObject>
#include <QDBusPendingReply>

#include "dockpluginmodel.h"

class QDBusInterface;
class QDBusMessage;

QDBusArgument &operator<<(QDBusArgument &arg, const DockItemInfo &info);
const QDBusArgument &operator>>(const QDBusArgument &arg, DockItemInfo &info);

Q_DECLARE_METATYPE(DockItemInfo)
Q_DECLARE_METATYPE(DockItemInfos)

class DockDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit DockDBusProxy(QObject *parent = nullptr);

    Q_PROPERTY(int DisplayMode READ displayMode WRITE setDisplayMode NOTIFY DisplayModeChanged)
    int displayMode();
    Q_INVOKABLE void setDisplayMode(int mode);

    Q_PROPERTY(int Position READ position WRITE setPosition NOTIFY PositionChanged)
    int position();
    Q_INVOKABLE void setPosition(int value);

    Q_PROPERTY(int HideMode READ hideMode WRITE setHideMode NOTIFY HideModeChanged)
    int hideMode();
    Q_INVOKABLE void setHideMode(int value);

    Q_PROPERTY(uint WindowSizeEfficient READ windowSizeEfficient WRITE setWindowSizeEfficient NOTIFY WindowSizeEfficientChanged)
    uint windowSizeEfficient();
    void setWindowSizeEfficient(uint value);

    Q_PROPERTY(uint WindowSizeFashion READ windowSizeFashion WRITE setWindowSizeFashion NOTIFY WindowSizeFashionChanged)
    uint windowSizeFashion();
    void setWindowSizeFashion(uint value);

    Q_PROPERTY(bool showInPrimary READ showInPrimary WRITE setShowInPrimary NOTIFY ShowInPrimaryChanged)
    bool showInPrimary();
    Q_INVOKABLE void setShowInPrimary(bool value);

    Q_PROPERTY(bool locked READ locked WRITE setLocked NOTIFY LockedChanged)
    bool locked();
    Q_INVOKABLE void setLocked(bool value);

    Q_PROPERTY(bool ShowRecent READ showRecent NOTIFY showRecentChanged)
    bool showRecent();

public Q_SLOTS:
    void resizeDock(int offset, bool dragging);
    QDBusPendingReply<QStringList> GetLoadedPlugins();
    QDBusPendingReply<QString> getPluginKey(const QString &pluginName);
    QDBusPendingReply<bool> getPluginVisible(const QString &pluginName);
    QDBusPendingReply<> setPluginVisible(const QString &pluginName, bool visible);
    QDBusPendingReply<> SetShowRecent(bool visible);
    QDBusPendingReply<DockItemInfos> plugins();
    QDBusPendingReply<> setItemOnDock(const QString settingKey, const QString &itemKey, bool visible);

Q_SIGNALS:
    // property changed signals
    void DisplayModeChanged(int displayMode) const;
    void PositionChanged(int position) const;
    void HideModeChanged(int hideMode) const;
    void WindowSizeEfficientChanged(uint windowSizeEfficient) const;
    void WindowSizeFashionChanged(uint windowSizeFashion) const;
    void ShowInPrimaryChanged(bool showInPrimary) const;
    void LockedChanged(bool locked) const;

    // real singals
    void pluginVisibleChanged(const QString &pluginName, bool visible) const;
    void showRecentChanged(bool) const;
    void pluginsChanged() const;

private:
    QDBusInterface *m_daemonDockInter;
    QDBusInterface *m_dockInter;
};

#endif // DOCKDBUSPROXY_H
