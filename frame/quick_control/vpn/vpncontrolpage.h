#ifndef VPNCONTROLPAGE_H
#define VPNCONTROLPAGE_H

#include "vpnlistmodel.h"

#include <QListView>

class VpnControlPage : public QListView
{
    Q_OBJECT

public:
    explicit VpnControlPage(dcc::network::NetworkModel *model, QWidget *parent = 0);

private:
    VpnListModel *m_listModel;
};

#endif // VPNCONTROLPAGE_H
