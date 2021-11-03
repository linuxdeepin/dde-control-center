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

#ifndef LOCALSERVER_H
#define LOCALSERVER_H
#include <QLocalServer>
#include <DSingleton>

class NetworkPanel;
class DockPopupWindow;
class LocalServer : public QObject
    , public Dtk::Core::DSingleton<LocalServer>
{
    Q_OBJECT

friend Dtk::Core::DSingleton<LocalServer>;

protected:
    LocalServer(QObject *parent = nullptr);

public:
    enum passwordChangeReason {
        Input,
        Cancel,
    };

    ~LocalServer();
    bool RunServer(const QString &serverName);
    void setWidget(NetworkPanel *panel, DockPopupWindow *popopWindow);
    bool changePassword(QString key, QString password, bool input);
    // 等待密码输入模式
    void setWaitPassword(bool isWait);
    void clear();
    static void release();

private Q_SLOTS:
    void newConnectionHandler();
    void disconnectedHandler();
    void readyReadHandler();

private:
    QLocalServer *m_server;
    QMap<QLocalSocket *, QString> m_clients;

    NetworkPanel *m_panel;
    DockPopupWindow *m_popopWindow;
    bool m_isWaitPassword;
};

#endif // LOCALSERVER_H
