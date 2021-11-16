/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
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

#ifndef LOCALCLIENT_H
#define LOCALCLIENT_H
#include <QLocalSocket>
#include <QProcess>

class QTimer;

class LocalClient : public QObject
{
    Q_OBJECT

public:
    LocalClient(const QString &serverName, QObject *parent = nullptr);
    ~LocalClient();

public:
    void setServerName(const QString &strServerName);
    bool ConnectToServer();
    void sendMessage(const QString &msg);
    void waitPassword(const QString &dev, const QString &ssid);

private Q_SLOTS:
    void connectedHandler();
    void disConnectedHandler();
    void readyReadHandler();

private:
    QLocalSocket *m_clinet;
    QString m_dev;
    QString m_ssid;
    QString m_serverName;
    QTimer *m_timer;
    QByteArray m_lastData;
};

#endif // LOCALCLIENT_H
