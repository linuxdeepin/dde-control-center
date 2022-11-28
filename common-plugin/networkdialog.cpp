// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "networkdialog.h"
#include "trayicon.h"
#include "networkdialog/networkpanel.h"

#include <QApplication>
#include <QDebug>
#include <QWidget>
#include <QWindow>
#include <QLocalServer>
#include <QLocalSocket>
#include <QJsonDocument>

NETWORKPLUGIN_USE_NAMESPACE

static QMap<QString, void (NetworkDialog::*)(QLocalSocket *, const QByteArray &)> s_FunMap = {
    { "password", &NetworkDialog::sendPassword },
    { "connect", &NetworkDialog::connectNetwork },
};

NetworkDialog::NetworkDialog(QObject *parent)
    : QObject(parent)
    , m_server(nullptr)
    , m_panel(new NetworkPanel())
{
    connect(m_panel, &NetworkPanel::passwordChanged, this, &NetworkDialog::changePassword);
}

NetworkDialog::~NetworkDialog()
{
}

void NetworkDialog::setServerName(const QString &name)
{
    if (m_server)
        return;

    m_serverName = name;
    m_server = new QLocalServer(this);
    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnectionHandler()));
    m_server->setSocketOptions(QLocalServer::WorldAccessOption);

    m_server->listen(m_serverName);
}

bool NetworkDialog::closeOnClear() const
{
    return m_panel->closeOnClear();
}

void NetworkDialog::setCloseOnClear(bool closeOnClear)
{
    m_panel->setCloseOnClear(closeOnClear);
}

void NetworkDialog::clear()
{
    m_panel->clear();
}

QWidget *NetworkDialog::panel()
{
    return m_panel->itemApplet();
}

void NetworkDialog::setConnectWireless(const QString &dev, const QString &ssid, bool wait)
{
    m_connectDev = dev;
    m_connectSsid = ssid;
    Q_EMIT requestShow();
    m_panel->passwordError(dev, ssid, wait);
}

void NetworkDialog::newConnectionHandler()
{
    QLocalSocket *socket = m_server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyReadHandler()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnectedHandler()));
    m_clients.append(socket);
}

void NetworkDialog::disconnectedHandler()
{
    QLocalSocket *socket = static_cast<QLocalSocket *>(sender());
    if (socket) {
        sendPassword(socket, "{}");
        m_clients.removeAll(socket);
        socket->deleteLater();
    }
}

void NetworkDialog::changePassword(const QString &key, const QString &password, bool input)
{
    QJsonObject json;
    json.insert("key", key);
    json.insert("password", password);
    json.insert("input", input);

    QJsonDocument doc;
    doc.setObject(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    for (auto it = m_clients.begin(); it != m_clients.end(); it++) {
        (*it)->write("\npassword:" + data + "\n");
    }
    Q_EMIT inputPassword(key, password, input);
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

void NetworkDialog::connectNetwork(QLocalSocket *socket, const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        setConnectWireless(obj.value("dev").toString(), obj.value("ssid").toString());
    }
    socket->write("\nreceive:" + data + "\n");
}

void NetworkDialog::sendPassword(QLocalSocket *socket, const QByteArray &data)
{
    Q_UNUSED(socket);
    QByteArray sendData = "\npassword:" + data + "\n";
    for (auto it = m_clients.begin(); it != m_clients.end(); it++) {
        (*it)->write(sendData);
    }
}
