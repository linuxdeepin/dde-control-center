/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     Tinalu Shao <shaotianlu@uniontech.com>
*
* Maintainer: Tinalu Shao <shaotianlu@uniontech.com>
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
#ifndef MONITORDBUSPROXY_H
#define MONITORDBUSPROXY_H

#include <QObject>
#include <QDBusPendingReply>
#include <QDBusReply>

class QDBusInterface;
class QDBusMessage;

class MonitorDBusProxy : public QObject
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.deepin.daemon.Display.Monitor"; }

public:
    explicit MonitorDBusProxy(QString monitorPath, QObject *parent = nullptr);
    Q_PROPERTY(QString Name READ name NOTIFY NameChanged)
    QString name();

private:
    void init();

public Q_SLOTS: // METHODS
    void onPropertiesChanged(const QDBusMessage &message);

Q_SIGNALS: // SIGNALS
    void NameChanged(const QString & value) const;

private:
    QDBusInterface *m_dBusMonitorInter;
    QDBusInterface *m_dBusMonitorPropertiesInter;
    QString m_monitorUserPath;
};

#endif // MONITORDBUSPROXY_H
