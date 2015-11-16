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
    m_manager->CheckNetworkConnectivity();

    auto reply = m_server->GetStatus();
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
    if (NetworkConnectivity::Disconnected == doCheckNetworkConnectivity()) {
        emit noNetwork();
        return;
    }

    // FIXME: the final code. stoped or uninitialized.
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
        return;
    case ServerStatus::Stoped:
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
    auto status = getStatus();
    if (status == ServerStatus::Sharing) {
        return;
    }

    if (status != ServerStatus::Disconnected) {
        onStatusChanged(status);
    }

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
