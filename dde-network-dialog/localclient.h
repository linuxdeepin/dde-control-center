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

#include <DSingleton>

class DockPopupWindow;
class NetworkPanel;

class QTimer;
class QTranslator;

class LocalClient : public QObject
    , public Dtk::Core::DSingleton<LocalClient>
{
    Q_OBJECT

    friend class Dtk::Core::DSingleton<LocalClient>;

public:
    enum RunReason {
        Lock,     // 锁屏插件唤起
        Greeter,  // greeter插件唤起
        Dock,     // 任务栏插件唤起
        Password, // 密码错误唤起
    };
    enum WaitClient {
        No,    // 无客户端等待
        Other, // 有其他客户端在等待密码
        Self,  // 本程序在等待密码
    };

protected:
    explicit LocalClient(QObject *parent = nullptr);

public:
    ~LocalClient();

public:
    bool ConnectToServer();
    void waitPassword(const QString &dev, const QString &ssid, bool wait);

    void showWidget();
    void initWidget();

    bool changePassword(QString key, QString password, bool input);

public:
    void showPosition(QLocalSocket *socket, const QByteArray &data);
    void receive(QLocalSocket *socket, const QByteArray &data);
    void connectNetwork(QLocalSocket *socket, const QByteArray &data);
    void receivePassword(QLocalSocket *socket, const QByteArray &data);
    void updateTranslator(QString locale);

private Q_SLOTS:
    void connectedHandler();
    void disConnectedHandler();
    void readyReadHandler();

private:
    QLocalSocket *m_clinet;
    QString m_dev;
    QString m_ssid; // 等待密码输入模式时才有值，由命令转入 @see waitPassword
    WaitClient m_wait; // 等待模式
    QTimer *m_timer;
    QTimer *m_exitTimer;
    QByteArray m_lastData;

    DockPopupWindow *m_popopWindow;
    NetworkPanel *m_panel;

    QTranslator *m_translator;
    QString m_locale;
};

#endif // LOCALCLIENT_H
