#ifndef VPNCONTROLPAGE_H
#define VPNCONTROLPAGE_H

#include <QFrame>

namespace dcc {

namespace network {

class NetworkModel;

}

}

class VpnControlPage : public QFrame
{
    Q_OBJECT

public:
    explicit VpnControlPage(dcc::network::NetworkModel *model, QWidget *parent = 0);

private:
    dcc::network::NetworkModel *m_networkModel;
};

#endif // VPNCONTROLPAGE_H
