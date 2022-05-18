/*
* Copyright (C) 2021 ~ 2022 Uniontech Software Technology Co.,Ltd.
*
* Author:     fanpengcheng <fanpengcheng@uniontech.com>
*
* Maintainer: fanpengcheng <fanpengcheng@uniontech.com>
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

#ifndef DAEMONDOCKDBUSPROXY_H
#define DAEMONDOCKDBUSPROXY_H

#include <QObject>
#include <QDBusPendingReply>

class QDBusInterface;
class QDBusMessage;
class DBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit DBusProxy(QObject *parent = nullptr);

    Q_PROPERTY(int DisplayMode READ displayMode WRITE setDisplayMode NOTIFY DisplayModeChanged)
    int displayMode();
    void setDisplayMode(int mode);

    Q_PROPERTY(int Position READ position WRITE setPosition NOTIFY PositionChanged)
    int position();
    void setPosition(int value);

    Q_PROPERTY(int HideMode READ hideMode WRITE setHideMode NOTIFY HideModeChanged)
    int hideMode();
    void setHideMode(int value);

    Q_PROPERTY(uint WindowSizeEfficient READ windowSizeEfficient WRITE setWindowSizeEfficient NOTIFY WindowSizeEfficientChanged)
    uint windowSizeEfficient();
    void setWindowSizeEfficient(uint value);

    Q_PROPERTY(uint WindowSizeFashion READ windowSizeFashion WRITE setWindowSizeFashion NOTIFY WindowSizeFashionChanged)
    uint windowSizeFashion();
    void setWindowSizeFashion(uint value);

    Q_PROPERTY(bool showInPrimary READ showInPrimary WRITE setShowInPrimary NOTIFY ShowInPrimaryChanged)
    bool showInPrimary();
    void setShowInPrimary(bool value);

public Q_SLOTS:
    void resizeDock(int offset, bool dragging);
    QDBusPendingReply<QStringList> GetLoadedPlugins();
    QDBusPendingReply<QString> getPluginKey(const QString &pluginName);
    QDBusPendingReply<bool> getPluginVisible(const QString &pluginName);
    QDBusPendingReply<> setPluginVisible(const QString &pluginName, bool visible);

Q_SIGNALS:
    // property changed signals
    void DisplayModeChanged(int displayMode) const;
    void PositionChanged(int position) const;
    void HideModeChanged(int hideMode) const;
    void WindowSizeEfficientChanged(uint windowSizeEfficient) const;
    void WindowSizeFashionChanged(uint windowSizeFashion) const;
    void ShowInPrimaryChanged(bool showInPrimary) const;

    // real singals
    void pluginVisibleChanged(const QString &pluginName, bool visible) const;

private slots:
    void onPropertiesChanged(const QDBusMessage &message);

private:
    QDBusInterface *m_daemonDockInter;
    QDBusInterface *m_dockInter;
};

#endif // DAEMONDOCKDBUSPROXY_H
