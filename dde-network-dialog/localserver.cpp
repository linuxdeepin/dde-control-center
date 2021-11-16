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
#include "localserver.h"
#include "dockpopupwindow.h"
#include "networkpanel.h"

#include <QLocalSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QApplication>

#define Normal 0
#define WaitPassword 1
// LocalServer及stdout通信数据格式为：cmd:{json}，并以\n分格，数据里不允许有\n
LocalServer::LocalServer(QObject *parent)
    : QObject(parent)
    , m_panel(nullptr)
    , m_popopWindow(nullptr)
    , m_isWaitPassword(false)
{
    m_server = new QLocalServer(this);
    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnectionHandler()));
    m_server->setSocketOptions(QLocalServer::UserAccessOption);
}

LocalServer::~LocalServer()
{
    clear();
}

void LocalServer::clear()
{
    if (m_server) {
        for (QLocalSocket *socket : m_clients.keys()) {
            socket->close();
            socket->deleteLater();
        }
        m_clients.clear();
        m_server->close();
        delete m_server;
        m_server = nullptr;
    }
}

void LocalServer::release()
{
    // 显性释放，否则下次创建会有问题
    LocalServer::instance()->clear();
}

void LocalServer::setWidget(NetworkPanel *panel, DockPopupWindow *popopWindow)
{
    m_panel = panel;
    m_popopWindow = popopWindow;
}

bool LocalServer::RunServer(const QString &serverName)
{
    return m_server->listen(serverName);
}

void LocalServer::newConnectionHandler()
{
    qInfo() << "New Connection";

    QLocalSocket *socket = m_server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyReadHandler()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnectedHandler()));
    m_clients.insert(socket, QString());
}

void LocalServer::readyReadHandler()
{
    QLocalSocket *socket = static_cast<QLocalSocket *>(sender());
    if (socket) {
        QByteArray dataRaw = socket->readAll();
        qInfo() << socket << dataRaw;
        QList<QByteArray> datas = dataRaw.split('\n');
        for (const QByteArray &data : datas) {
            if (data.startsWith("show:")) {
                QByteArray cmd = data.mid(5);
                QJsonDocument doc = QJsonDocument::fromJson(cmd);
                if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    int x = obj.value("x").toInt();
                    int y = obj.value("y").toInt();
                    QString positionStr = obj.value("direction").toString();
                    DArrowRectangle::ArrowDirection position = DArrowRectangle::ArrowBottom;
                    if (!positionStr.isEmpty()) {
                        switch (positionStr.at(0).toLower().toLatin1()) {
                        case 'b':
                            position = DArrowRectangle::ArrowBottom;
                            break;
                        case 't':
                            position = DArrowRectangle::ArrowTop;
                            break;
                        case 'l':
                            position = DArrowRectangle::ArrowLeft;
                            break;
                        case 'r':
                            position = DArrowRectangle::ArrowRight;
                            break;
                        }
                    }
                    if (0 != x && 0 != y) {
                        m_popopWindow->setArrowDirection(position);
                        m_popopWindow->show(x, y);
                    }
                }
            } else if (data.startsWith("connect:")) {
                QByteArray cmd = data.mid(8);
                QJsonDocument doc = QJsonDocument::fromJson(cmd);
                if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    QString dev = obj.value("dev").toString();
                    QString ssid = obj.value("ssid").toString();
                    m_panel->passwordError(dev, ssid);
                    m_clients[socket] = ssid;

                    QJsonObject json;
                    json.insert("ssid", ssid);

                    QJsonDocument doc;
                    doc.setObject(json);
                    QString data = "\nreceive:" + doc.toJson(QJsonDocument::Compact) + "\n";
                    socket->write(data.toUtf8());
                    socket->flush();
                }
            }
        }
    }
}

void LocalServer::disconnectedHandler()
{
    QLocalSocket *socket = static_cast<QLocalSocket *>(sender());
    if (socket) {
        m_clients.remove(socket);
        socket->deleteLater();
    }
}

void LocalServer::setWaitPassword(bool isWait)
{
    m_isWaitPassword = isWait;
}

bool LocalServer::changePassword(QString key, QString password, bool input)
{
    bool ret = false;
    if (m_isWaitPassword || m_clients.values().contains(key)) {
        QJsonObject json;
        json.insert("key", key);
        json.insert("password", password);
        json.insert("input", input);

        QJsonDocument doc;
        doc.setObject(json);
        QString data = "\npassword:" + doc.toJson(QJsonDocument::Compact) + "\n";
        for (auto client = m_clients.begin(); client != m_clients.end(); client++) {
            if (client.value() == key) {
                QLocalSocket *socket = client.key();
                socket->write(data.toUtf8());
                socket->flush();
                ret = true;
            }
        }
        if (m_isWaitPassword) {
            QFile file;
            if (!file.open(stdout, QFile::WriteOnly)) {
                qInfo() << "open STDOUT failed";
                qApp->exit(-4);
            }
            file.write("\n" + data.toUtf8() + "\n");
            file.flush();
            file.close();

            qInfo() << "\n" + data.toUtf8() + "\n";
        }
    }
    return ret;
}
