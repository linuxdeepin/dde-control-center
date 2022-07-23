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

#include "networkdialog.h"
#include "trayicon.h"

#include <QApplication>
#include <QDebug>
#include <QWidget>
#include <QWindow>
#include <QLocalServer>
#include <QLocalSocket>
#include <QJsonDocument>

#include <unistd.h>
#include <pwd.h>

NETWORKPLUGIN_USE_NAMESPACE

static const QString NetworkDialogApp = "dde-network-dialog"; //网络列表执行文件
static QMap<QString, void (NetworkDialog::*)(QLocalSocket *, const QByteArray &)> s_FunMap = {
    { "show", &NetworkDialog::showDialog },
    { "password", &NetworkDialog::sendPassword },
    { "connect", &NetworkDialog::connectNetwork },
    { "close", &NetworkDialog::closeServer },
    { "start", &NetworkDialog::startServer },
    { "grabKeyboard", &NetworkDialog::onGrabKeyboard }
};

/**
 * @brief The ClientType enum
 * 客户端类型
 */
enum ClientType {
    Unknown,  // 未知
    Show,     // 弹窗展示
    Password, // 等待密码
};

NetworkDialog::NetworkDialog(QObject *parent)
    : QObject(parent)
    , m_x(0)
    , m_y(0)
    , m_position(Dtk::Widget::DArrowRectangle::ArrowDirection::ArrowBottom)
    , m_runReason(Dock)
    , m_process(new QProcess(this))
    , m_focusWidget(nullptr)
    , m_saveMode(false)
    , m_serverName(NetworkDialogApp + QString::number(getuid()))
    , m_visible(false)
{
    m_server = new QLocalServer(this);
    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnectionHandler()));
    m_server->setSocketOptions(QLocalServer::WorldAccessOption);

    connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &NetworkDialog::finished);
}

NetworkDialog::~NetworkDialog()
{
    m_process->close();
}

void NetworkDialog::finished(int, QProcess::ExitStatus)
{
    freeFocus();
    setVisible(false);
}

void NetworkDialog::show()
{
    QTimer::singleShot(0, this, [ this ] {
        m_process->blockSignals(true);
        m_process->close();
        m_process->blockSignals(false);
        if (Greeter == m_runReason)
            requestFocus();
        runProcess(true);
    });
}

void NetworkDialog::requestFocus()
{
    for (QWidget *w : qApp->topLevelWidgets()) {
        // 需要判断contentVisible属性是否为true，否则在多屏的情况下，
        // 释放grab的时候可能会让隐藏的屏幕抓取键盘，显示登陆界面的屏幕无法获取到焦点
        if (QString("FullscreenBackground") == w->metaObject()->superClass()->className() && w->property("contentVisible").toBool()) {
            w->installEventFilter(this);
            if (w->window() && w->window()->windowHandle()->setKeyboardGrabEnabled(false)) {
                qInfo() << "requestFocus true";
            }
            m_focusWidget = w;
        }
    }
}

void NetworkDialog::freeFocus()
{
    if (m_focusWidget) {
        m_focusWidget->removeEventFilter(this);
        if (m_focusWidget->window() && m_focusWidget->window()->windowHandle()->setKeyboardGrabEnabled(true)) {
            qInfo() << "freeFocus true";
        }
        m_focusWidget = nullptr;
    }
}

bool NetworkDialog::eventFilter(QObject *watched, QEvent *e)
{
    if (qobject_cast<TrayIcon *>(watched)) {
        switch (e->type()) {
        case QEvent::Show:
            // 在事件循环中处理,因为位置有可能没有及时更新
            QTimer::singleShot(0, this, [this] {
                for (auto it = m_clients.begin(); it != m_clients.end(); it++)
                    forceShowDialog(it.key());
            });
            break;
        case QEvent::Hide:
            // 网络插件隐藏的时候关闭弹窗
            // 主要是为了规避锁屏和网络列表争抢键盘的问题（可能会导致锁屏失败，退出进程）
            closeDialog();
            break;
        default:
            break;
        }
    }

    if (e->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
        QString data = QString("\nclick:{\"x\":%1,\"y\":%2}\n").arg(mouseEvent->x()).arg(mouseEvent->y());
        QByteArray bdata = data.toUtf8();
        for (auto it = m_clients.begin(); it != m_clients.end(); it++)
            it.key()->write(bdata);
    }

    if (watched == m_focusWidget && e->type() == QEvent::WindowDeactivate) {
        return true;
    }

    return QObject::eventFilter(watched, e);
}

void NetworkDialog::runProcess(bool show)
{
    QStringList argList;
    if (show) {
        argList << "-s" << showConfig();
    }
    if (!m_connectSsid.isEmpty()) {
        argList << "-c" << m_connectSsid << "-n" << m_connectDev;
        m_connectSsid.clear();
    }
    m_process->start(NetworkDialogApp, argList);
}

void NetworkDialog::setConnectWireless(const QString &dev, const QString &ssid)
{
    if (!m_server->isListening())
        return;

    m_connectDev = dev;
    m_connectSsid = ssid;
    for (auto it = m_clients.begin(); it != m_clients.end(); it++) {
        if (it.value() == ClientType::Show) {
            it.key()->write(QString("\nconnect:{\"ssid\":\"%1\",\"dev\":\"%2\"}\n").arg(m_connectSsid).arg(m_connectDev).toUtf8());
            m_connectSsid.clear();
            return;
        }
    }
    runProcess(false);
}

void NetworkDialog::setRunReason(RunReason reason)
{
    m_runReason = reason;
}

void NetworkDialog::setPosition(int x, int y, Dtk::Widget::DArrowRectangle::ArrowDirection position)
{
    m_x = x;
    m_y = y;
    m_position = position;
}

bool NetworkDialog::isVisible() const
{
    return m_visible;
}

void NetworkDialog::setLocale(const QString &locale)
{
    m_locale = locale;
}

void NetworkDialog::runServer(bool start)
{
    if (!start)
        return;

    m_server->close();
    QLocalServer::removeServer(m_serverName);
    if (m_server->listen(m_serverName))
        return;

    QLocalSocket *socket = new QLocalSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyReadHandler()));
    connect(socket, &QLocalSocket::connected, this, [ socket ]() {
        socket->write("\nclose:{}\n");
    });
    connect(socket, &QLocalSocket::disconnected, this, [ this, socket ]() {
        socket->deleteLater();
        QLocalServer::removeServer(m_serverName);
        m_server->listen(m_serverName);
    });
    socket->connectToServer(m_serverName);
}

void NetworkDialog::newConnectionHandler()
{
    QLocalSocket *socket = m_server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyReadHandler()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnectedHandler()));
    m_clients.insert(socket, ClientType::Unknown);
    setVisible(true);
}

void NetworkDialog::disconnectedHandler()
{
    QLocalSocket *socket = static_cast<QLocalSocket *>(sender());
    if (socket) {
        sendPassword(socket, "{}");
        m_clients.remove(socket);
        socket->deleteLater();
    }
    if (m_clients.isEmpty()) {
        freeFocus();
        setVisible(false);
    }
}

void NetworkDialog::setVisible(bool visible)
{
    if (visible) {
        m_visible = true;
    } else {
        // 延时设置false
        QTimer::singleShot(200, this, [ this ]() {
            m_visible = false;
        });
    }
}
void NetworkDialog::readyReadHandler()
{
    QLocalSocket *socket = static_cast<QLocalSocket *>(sender());
    if (socket) {
        QByteArray allData = socket->readAll();
        allData = m_lastData + allData;
        qDebug() << "Recieve data from client: " << allData;
        QList<QByteArray> dataArray = allData.split('\n');
        m_lastData = dataArray.last();
        for (const QByteArray &data : dataArray) {
            int keyIndex = data.indexOf(':');
            if (keyIndex != -1) {
                QString key = data.left(keyIndex);
                QByteArray value = data.mid(keyIndex + 1);
                if (s_FunMap.contains(key)) {
                    (this->*s_FunMap.value(key))(socket, value);
                }
            }
        }
    }
}

QByteArray NetworkDialog::showConfig()
{
    QJsonObject json;
    json.insert("x", m_x);
    json.insert("y", m_y);
    json.insert("reason", m_runReason);
    json.insert("position", m_position);
    json.insert("locale", m_locale);
    QJsonDocument doc;
    doc.setObject(json);
    return doc.toJson(QJsonDocument::Compact);
}

void NetworkDialog::showDialog(QLocalSocket *socket, const QByteArray &)
{
    emit requestPosition();
    if (Greeter == m_runReason)
        requestFocus();
    m_clients[socket] = ClientType::Show;
    socket->write("\nshowPosition:" + showConfig() + "\n");
}

void NetworkDialog::onGrabKeyboard(QLocalSocket *socket, const QByteArray &data)
{
    Q_UNUSED(socket)
    Q_UNUSED(data)

    requestFocus();
}

void NetworkDialog::forceShowDialog(QLocalSocket *socket)
{
    emit requestPosition();
    if (Greeter == m_runReason)
        requestFocus();
    m_clients[socket] = ClientType::Show;

    QJsonObject json;
    json.insert("x", m_x);
    json.insert("y", m_y);
    json.insert("reason", m_runReason);
    json.insert("position", m_position);
    json.insert("locale", m_locale);
    json.insert("force", true);
    QJsonDocument doc;
    doc.setObject(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    socket->write("\nshowPosition:" + data + "\n");
}

void NetworkDialog::closeDialog()
{
    for (auto it = m_clients.begin(); it != m_clients.end(); it++)
        closeDialog(it.key());
}

void NetworkDialog::closeDialog(QLocalSocket *socket)
{
    socket->write("\nclose:" + QByteArray() + "\n");
}

void NetworkDialog::connectNetwork(QLocalSocket *socket, const QByteArray &data)
{
    QLocalSocket *showSocket = nullptr;
    for (auto it = m_clients.begin(); it != m_clients.end(); it++) {
        if (it.value() == ClientType::Show) {
            showSocket = it.key();
            break;
        }
    }
    if (nullptr == showSocket) {
        showDialog(socket, QByteArray());
        showSocket = socket;
    }
    showSocket->write("\nconnect:" + data + "\n");
    socket->write("\nreceive:" + data + "\n");
}

void NetworkDialog::sendPassword(QLocalSocket *socket, const QByteArray &data)
{
    Q_UNUSED(socket);
    QByteArray sendData = "\npassword:" + data + "\n";
    for (auto it = m_clients.begin(); it != m_clients.end(); it++) {
        it.key()->write(sendData);
    }
}

void NetworkDialog::closeServer(QLocalSocket *socket, const QByteArray &data)
{
    Q_UNUSED(data);
    socket->write("\nstart:{}\n");
    m_server->close();
    QLocalServer::removeServer(m_serverName);
}

void NetworkDialog::startServer(QLocalSocket *socket, const QByteArray &data)
{
    Q_UNUSED(data);
    socket->disconnectFromServer();
    m_server->close();
    QLocalServer::removeServer(m_serverName);
    m_server->listen(m_serverName);
}

void NetworkDialog::updateDialogPosition()
{
    for (auto it = m_clients.begin(); it != m_clients.end(); it++)
        showDialog(it.key(), QByteArray());
}
