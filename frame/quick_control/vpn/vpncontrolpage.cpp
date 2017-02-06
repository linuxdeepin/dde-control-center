#include "vpncontrolpage.h"

#include <QDebug>

using dcc::network::NetworkModel;

VpnControlPage::VpnControlPage(NetworkModel *model, QWidget *parent)
    : QListView(parent),

      m_listModel(new VpnListModel(model, this))
{
    setFrameStyle(QFrame::NoFrame);
    setModel(m_listModel);
}
