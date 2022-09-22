// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
    void setLocale(const QString &locale);
    void closeDialog();
    void setServerName(const QString &name);

private:
    void runProcess(bool show = true);
    bool eventFilter(QObject *watched, QEvent *e) override;
    void setVisible(bool visible);
    void forceShowDialog(QLocalSocket *socket);

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
    void onGrabKeyboard(QLocalSocket *socket, const QByteArray &data);
    void closeDialog(QLocalSocket *socket);
    QByteArray showConfig();
    void updateDialogPosition();

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
    bool m_visible;
    QString m_locale;
    bool m_start;
};

NETWORKPLUGIN_END_NAMESPACE
#endif // NETWORKDIALOG_H
