// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NETWORKDIALOG_H
#define NETWORKDIALOG_H

#include "utils.h"

class QWidget;
class QLocalServer;
class QLocalSocket;
class NetworkPanel;

NETWORKPLUGIN_BEGIN_NAMESPACE

class NetworkDialog : public QObject
{
    Q_OBJECT
public:
    explicit NetworkDialog(QObject *parent = Q_NULLPTR);
    ~NetworkDialog() override;

Q_SIGNALS:
    void requestShow();
    void inputPassword(const QString &key, const QString &password, bool input);

public:
    QWidget *panel();
    void setConnectWireless(const QString &dev, const QString &ssid);
    void setServerName(const QString &name);

    bool closeOnClear() const;
    void setCloseOnClear(bool closeOnClear);
    void clear();

private Q_SLOTS:
    void newConnectionHandler();
    void readyReadHandler();
    void disconnectedHandler();
    void changePassword(const QString &key, const QString &password, bool input);

public:
    void connectNetwork(QLocalSocket *socket, const QByteArray &data);
    void sendPassword(QLocalSocket *socket, const QByteArray &data);

private:
    QString m_connectDev;
    QString m_connectSsid;
    QByteArray m_lastData;

    QLocalServer *m_server;
    QList<QLocalSocket *> m_clients;
    QString m_serverName;

    NetworkPanel *m_panel;
};

NETWORKPLUGIN_END_NAMESPACE
#endif // NETWORKDIALOG_H
