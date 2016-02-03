/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "access.h"
#include <QDebug>

#define RETURN_IF_NO_NETWORK(status) if (status == NetworkConnectivity::Disconnected) { emit noNetwork(); return ;}

AccessController::AccessController(com::deepin::daemon::Remoting::Manager* manager,
                                   com::deepin::daemon::Remoting::Client* client,
                                   QObject*p)
        : IAccessController(p),
          m_manager(manager),
          m_client(client)
{
}

int AccessController::getStatus()
{
    auto reply = m_client->GetStatus();
    reply.waitForFinished();
    return reply.value();
}

bool AccessController::isAlreadyConnected()
{
    return getStatus() == ClientStatus::ConnectOk;
}

void AccessController::initStatus()
{
    RETURN_IF_NO_NETWORK(doCheckNetworkConnectivity());
    auto status = getStatus();
    onStatusChanged(status);
}

void AccessController::checkNetworkConnectivity()
{
    RETURN_IF_NO_NETWORK(doCheckNetworkConnectivity());
}

int AccessController::doCheckNetworkConnectivity()
{
    auto reply = m_manager->CheckNetworkConnectivity();
    reply.waitForFinished();
    return reply.value();
}

void AccessController::connect(QString accessToken)
{
    RETURN_IF_NO_NETWORK(doCheckNetworkConnectivity());

    auto status = getStatus();
    if (status == ClientStatus::ConnectOk) {
        emit connected();
        return;
    }

    m_client->Start();

    m_accessToken = accessToken;
    QObject::connect(m_client, SIGNAL(StatusChanged(int)), this, SLOT(onStatusChanged(int)));
}

void AccessController::retry()
{
    m_client->Start();
}

void AccessController::disconnect()
{
    m_client->Stop();
}

void AccessController::onStatusChanged(int status)
{
    if (NetworkConnectivity::Disconnected == doCheckNetworkConnectivity()) {
        emit  noNetwork();
        return;
    }

    switch (status) {
    case ClientStatus::Unintialized:
        return;
    case ClientStatus::Started:
        emit connecting();
        return;
    case ClientStatus::Stopped:
        emit stopped();
        break;
    case ClientStatus::PageReady:
        m_client->Connect(m_accessToken);
        return;
    case ClientStatus::Connecting:
        return;
    case ClientStatus::ConnectOk:
        emit connected();
        break;
    case ClientStatus::ConnectServerFailed:
        emit connectFailed(AccessError::ConnectServerFailed);
        break;
    case ClientStatus::InvalidToken:
        emit connectFailed(AccessError::InvalidToken);
        break;
    case ClientStatus::Disconnected:
        emit disconnected();
        break;
    }
}
