#include "vpncontrolpage.h"

#include "network/networkmodel.h"

#include <QDebug>

using dcc::network::NetworkModel;

VpnControlPage::VpnControlPage(NetworkModel *model, QWidget *parent)
    : QFrame(parent)
{
    setStyleSheet("background-color: red;");

    qDebug() << model->vpns();
}
