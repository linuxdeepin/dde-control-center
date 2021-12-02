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
#include "localclient.h"

#include "networkpanel.h"
#include "dockpopupwindow.h"
#include "thememanager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>
#include <QTimer>
#include <QProcess>
#include <QFile>

#include <networkcontroller.h>

const QString NetworkDialogApp = "dde-network-dialog"; //网络列表执行文件
static QMap<QString, void (LocalClient::*)(QLocalSocket *, const QByteArray &)> s_FunMap = {
    { "showPosition", &LocalClient::showPosition },
    { "connect", &LocalClient::connectNetwork },
    { "password", &LocalClient::receivePassword },
    { "receive", &LocalClient::receive }
};

LocalClient::LocalClient(QObject *parent)
    : QObject(parent)
    , m_wait(false)
    , m_timer(nullptr)
    , m_exitTimer(new QTimer(this))
    , m_popopWindow(nullptr)
    , m_panel(nullptr)
{
    m_clinet = new QLocalSocket(this);
    connect(m_clinet, SIGNAL(connected()), this, SLOT(connectedHandler()));
    connect(m_clinet, SIGNAL(disconnected()), this, SLOT(disConnectedHandler()));
    connect(m_clinet, SIGNAL(readyRead()), this, SLOT(readyReadHandler()));
    connect(m_exitTimer, SIGNAL(timeout()), qApp, SLOT(quit()));
    ConnectToServer();
}

LocalClient::~LocalClient()
{
    m_clinet->disconnectFromServer();
    delete m_clinet;
}

void LocalClient::connectedHandler()
{
    if (m_clinet->isOpen()) {
        if (m_exitTimer) {
            m_exitTimer->stop();
            m_exitTimer->deleteLater();
            m_exitTimer = nullptr;
        }
        if (!m_ssid.isEmpty()) {
            QJsonObject json;
            json.insert("dev", m_dev);
            json.insert("ssid", m_ssid);
            QJsonDocument doc;
            doc.setObject(json);
            QString data = "\nconnect:" + doc.toJson(QJsonDocument::Compact) + "\n";
            m_clinet->write(data.toUtf8());
            m_clinet->flush();
        }
    }
}

void LocalClient::disConnectedHandler()
{
    qApp->exit(0);
}

bool LocalClient::ConnectToServer()
{
    m_exitTimer->start(10000);
    m_clinet->connectToServer(NetworkDialogApp + QString::number(getuid()));
    m_clinet->waitForConnected();
    QLocalSocket::LocalSocketState state = m_clinet->state();
    return state == QLocalSocket::ConnectedState;
}

void LocalClient::readyReadHandler()
{
    QLocalSocket *socket = static_cast<QLocalSocket *>(sender());
    if (!socket)
        return;

    QByteArray allData = socket->readAll();
    allData = m_lastData + allData;
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

void LocalClient::waitPassword(const QString &dev, const QString &ssid, bool wait)
{
    m_wait = wait;
    m_ssid = ssid;
    m_dev = dev;
    if (m_panel) {
        m_panel->passwordError(m_dev, m_ssid);
        return;
    }
    if (!m_timer) {
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &LocalClient::connectedHandler);
        m_timer->setInterval(1000);
    }
    if (m_clinet->isOpen()) {
        connectedHandler();
    }

    m_timer->start();
}

void LocalClient::showWidget()
{
    m_clinet->write("\nshow:{}\n");
    initWidget();
}

void LocalClient::initWidget()
{
    if (!m_popopWindow) {
        m_popopWindow = new DockPopupWindow();
        m_panel = new NetworkPanel(m_popopWindow);
        m_popopWindow->setContent(m_panel->itemApplet());
        QObject::connect(qApp, &QCoreApplication::destroyed, m_popopWindow, &DockPopupWindow::deleteLater);
        QObject::connect(m_popopWindow, &DockPopupWindow::hideSignal, qApp, &QCoreApplication::quit);
        QObject::connect(m_popopWindow, &DockPopupWindow::hideSignal, m_popopWindow, &DockPopupWindow::deleteLater);
    }
}

void LocalClient::showPosition(QLocalSocket *socket, const QByteArray &data)
{
    Q_UNUSED(socket)
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        int x = obj.value("x").toInt();
        int y = obj.value("y").toInt();
        int reason = obj.value("reason").toInt();
        int position = obj.value("position").toInt();

        switch (reason) {
        case Greeter:
            dde::network::NetworkController::setServiceType(dde::network::ServiceLoadType::LoadFromManager);
            ThemeManager::instance()->setThemeType(ThemeManager::GreeterType);
            break;
        case Lock:
            ThemeManager::instance()->setThemeType(ThemeManager::LockType);
            break;
        default:
            break;
        }
        initWidget();
        m_popopWindow->setArrowDirection(static_cast<DArrowRectangle::ArrowDirection>(position));
        m_popopWindow->show(x, y);
    }
}

void LocalClient::connectNetwork(QLocalSocket *socket, const QByteArray &data)
{
    Q_UNUSED(socket)
    initWidget();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        m_panel->passwordError(obj.value("dev").toString(), obj.value("ssid").toString());
    }
}

void LocalClient::receivePassword(QLocalSocket *socket, const QByteArray &data)
{
    Q_UNUSED(socket)
    if (m_wait) {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            QFile file;
            if (!file.open(stdout, QFile::WriteOnly)) {
                qInfo() << "open STDOUT failed";
                qApp->exit(-4);
            }
            file.write("\npassword:" + data + "\n");
            file.flush();
            file.close();
            m_ssid.clear();
            m_wait = false;
            qApp->exit(doc.object().value("input").toBool() ? 0 : 1);
        }
    }
}

void LocalClient::receive(QLocalSocket *socket, const QByteArray &data)
{
    Q_UNUSED(socket)
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        QString ssid = obj.value("ssid").toString();
        if (ssid == m_ssid) {
            m_timer->stop();
        }
    }
}

bool LocalClient::changePassword(QString key, QString password, bool input)
{
    QJsonObject json;
    json.insert("key", key);
    json.insert("password", password);
    json.insert("input", input);

    QJsonDocument doc;
    doc.setObject(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    receivePassword(nullptr, data);
    m_clinet->write("\npassword:" + data + "\n");
    return m_wait;
}
