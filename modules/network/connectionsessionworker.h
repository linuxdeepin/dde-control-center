#ifndef CONNECTIONSESSIONWORKER_H
#define CONNECTIONSESSIONWORKER_H

#include <QObject>

#include <com_deepin_daemon_network_connectionsession.h>

using ConnectionSessionInter = com::deepin::daemon::ConnectionSession;

namespace dcc {

namespace network {

class ConnectionSessionModel;
class ConnectionSessionWorker : public QObject
{
    Q_OBJECT

public:
    explicit ConnectionSessionWorker(const QString &sessionPath, ConnectionSessionModel *model, QObject *parent = 0);

private:
    ConnectionSessionModel *m_connModel;

    ConnectionSessionInter m_sessionInter;
};

}

}

#endif // CONNECTIONSESSIONWORKER_H
