#ifndef NETWORKWORKER_H
#define NETWORKWORKER_H

#include <QObject>

#include <com_deepin_daemon_network.h>

namespace dcc {

namespace network {

using NetworkInter = com::deepin::daemon::Network;

class NetworkModel;
class NetworkWorker : public QObject
{
    Q_OBJECT

public:
    explicit NetworkWorker(NetworkModel *model, QObject *parent = 0);

private:
    NetworkInter m_networkInter;

    NetworkModel *m_networkModel;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKWORKER_H
