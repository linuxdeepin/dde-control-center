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

#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>
#include <QTimer>
#include <QProcess>
#include <QFile>

const QString NetworkDialogApp = "dde-network-dialog"; //网络列表执行文件

LocalClient::LocalClient(const QString &serverName, QObject *parent)
    : QObject(parent)
    , m_serverName(serverName)
    , m_timer(nullptr)
{
    m_clinet = new QLocalSocket(this);
    connect(m_clinet, SIGNAL(connected()), this, SLOT(connectedHandler()));
    connect(m_clinet, SIGNAL(disconnected()), this, SLOT(disConnectedHandler()));
    connect(m_clinet, SIGNAL(readyRead()), this, SLOT(readyReadHandler()));
}

LocalClient::~LocalClient()
{
    m_clinet->disconnectFromServer();
    delete m_clinet;
}

void LocalClient::connectedHandler()
{
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

void LocalClient::disConnectedHandler()
{
    qApp->exit(0);
}

void LocalClient::setServerName(const QString &strServerName)
{
    m_serverName = strServerName;
}

bool LocalClient::ConnectToServer()
{
    m_clinet->connectToServer(m_serverName);
    m_clinet->waitForConnected();
    QLocalSocket::LocalSocketState state = m_clinet->state();
    return state == QLocalSocket::ConnectedState;
}

void LocalClient::sendMessage(const QString &msg)
{
    m_clinet->write(msg.toStdString().c_str());
    m_clinet->flush();

    if (!m_clinet->bytesAvailable())
        m_clinet->waitForReadyRead();

    QTextStream stream(m_clinet);
    QString respond = stream.readAll();
    qInfo() << "Read Data From Server:" << respond;
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
        if (data.startsWith("password:")) {
            QByteArray cmd = data.mid(9);
            QJsonDocument doc = QJsonDocument::fromJson(cmd);
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                QString key = obj.value("key").toString();
                QString password = obj.value("password").toString();
                bool input = obj.value("input").toBool();
                QFile file;
                if (!file.open(stdout, QFile::WriteOnly)) {
                    qInfo() << "open STDOUT failed";
                    qApp->exit(-4);
                }
                file.write("\n" + data + "\n");
                file.flush();
                file.close();

                qApp->exit(input ? 0 : 1);
            }
        } else if (data.startsWith("receive:")) { // 已收到
            QByteArray cmd = data.mid(8);
            QJsonDocument doc = QJsonDocument::fromJson(cmd);
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                QString ssid = obj.value("ssid").toString();
                if (ssid == m_ssid) {
                    m_ssid.clear();
                    m_dev.clear();
                    m_timer->stop();
                }
            }
        }
    }
}

void LocalClient::waitPassword(const QString &dev, const QString &ssid)
{
    m_ssid = ssid;
    m_dev = dev;
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
