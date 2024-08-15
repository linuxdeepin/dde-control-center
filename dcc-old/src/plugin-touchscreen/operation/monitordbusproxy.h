//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
    { return "org.deepin.dde.Display1.Monitor"; }

public:
    explicit MonitorDBusProxy(QString monitorPath, QObject *parent = nullptr);
    Q_PROPERTY(QString Name READ name NOTIFY NameChanged)
    QString name();

private:
    void init();

Q_SIGNALS: // SIGNALS
    void NameChanged(const QString & value) const;

private:
    QDBusInterface *m_dBusMonitorInter;
    QDBusInterface *m_dBusMonitorPropertiesInter;
    QString m_monitorUserPath;
};

#endif // MONITORDBUSPROXY_H
