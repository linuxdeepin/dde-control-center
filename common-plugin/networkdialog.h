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

#ifndef NETWORKDIALOG_H
#define NETWORKDIALOG_H

#include "utils.h"

#include <QProcess>

#include <DArrowRectangle>

class QWidget;
class QLocalServer;
class QLocalSocket;

NETWORKPLUGIN_BEGIN_NAMESPACE

class NetworkDialog : public QObject
{
    Q_OBJECT

signals:
    void requestPosition();

public:
    enum RunReason {
        Lock,     // 锁屏插件唤起
        Greeter,  // greeter插件唤起
        Dock,     // 任务栏插件唤起
        Password, // 密码错误唤起
    };

    explicit NetworkDialog(QObject *parent = Q_NULLPTR);
    ~NetworkDialog() override;

    void show();
    void setConnectWireless(const QString &dev, const QString &ssid);
    void setRunReason(RunReason reason);
    void setPosition(int x, int y, Dtk::Widget::DArrowRectangle::ArrowDirection position = Dtk::Widget::DArrowRectangle::ArrowDirection::ArrowBottom);
    bool isVisible() const;

private:
    void runProcess(bool show = true);
    bool eventFilter(QObject *watched, QEvent *e) override;

public Q_SLOTS:
    void runServer(bool start);

private Q_SLOTS:
    void finished(int, QProcess::ExitStatus);
    void requestFocus();
    void freeFocus();
    void newConnectionHandler();
    void readyReadHandler();
    void disconnectedHandler();

public:
    void showDialog(QLocalSocket *socket, const QByteArray &data);
    void connectNetwork(QLocalSocket *socket, const QByteArray &data);
    void sendPassword(QLocalSocket *socket, const QByteArray &data);
    void closeServer(QLocalSocket *socket, const QByteArray &data);
    void startServer(QLocalSocket *socket, const QByteArray &data);
    QByteArray showConfig();

private:
    int m_x;
    int m_y;
    Dtk::Widget::DArrowRectangle::ArrowDirection m_position;
    RunReason m_runReason;

    QProcess *m_process;
    QWidget *m_focusWidget;
    QString m_connectDev;
    QString m_connectSsid;
    bool m_saveMode;
    QByteArray m_lastData;

    QLocalServer *m_server;
    QMap<QLocalSocket *, int> m_clients;
    QString m_serverName;
};

NETWORKPLUGIN_END_NAMESPACE
#endif // NETWORKDIALOG_H
