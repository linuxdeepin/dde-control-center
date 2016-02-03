/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "share.h"

#include <QDebug>

ShareController::ShareController(com::deepin::daemon::Remoting::Manager* manager,
        com::deepin::daemon::Remoting::Server* server,
        QObject* p)
        : IShareController(p),
          m_manager(manager),
          m_server(server)
{
}

ShareController::~ShareController() {
    m_server->deleteLater();
}

void ShareController::retry()
{
    startGenAccessToken();
}

QString ShareController::getPeerID()
{
    auto reply = m_server->GetPeerId();
    reply.waitForFinished();
    return reply.value();
}

void ShareController::checkNetworkConnectivity()
{
    if (doCheckNetworkConnectivity() == NetworkConnectivity::Disconnected)
        emit noNetwork();
}

int ShareController::doCheckNetworkConnectivity()
{
    auto reply = m_manager->CheckNetworkConnectivity();
    reply.waitForFinished();
    return reply.value();
}

int ShareController::getStatus()
{
    auto reply = m_server->GetStatus();
    reply.waitForFinished();
    return reply.value();
}

void ShareController::onStatusChanged(int status)
{
    int networkState = doCheckNetworkConnectivity();
    if (NetworkConnectivity::Disconnected == networkState ) {
        emit noNetwork();
        return;
    }

    switch (status) {
    case ServerStatus::Uninitialized:
        return;
    case ServerStatus::Started:
        emit generatingAccessToken();
        return;
    case ServerStatus::PeerIDOk:
        emit genAccessTokenSuccessed(getPeerID());
        return;
    case ServerStatus::PeerIDFailed:
        emit genAccessTokenFailed();
        return;
    case ServerStatus::Sharing:
        emit sharing();
        return;
    case ServerStatus::Stopped:
        emit stopped();
        return;
    case ServerStatus::Disconnected:
        emit disconnected();
        return;
    }
}

bool ShareController::isSharing()
{
    return getStatus() == ServerStatus::Sharing;
}

void ShareController::startGenAccessToken()
{
    qDebug() << "startGenAccessToken";
    auto status = getStatus();
    if (status == ServerStatus::Sharing) {
        return;
    }

    if (status != ServerStatus::Disconnected) {
        onStatusChanged(status);
    }

    qDebug() << "listen StatusChanged signal";
    QObject::connect(m_server, SIGNAL(StatusChanged(int)), this, SLOT(onStatusChanged(int)));
    m_server->Start();
}

void ShareController::cancel()
{
    m_server->Stop();
}

void ShareController::disconnect()
{
    m_server->StopNotify();
}
