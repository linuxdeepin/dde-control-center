#include "vpncontrolpage.h"
#include "../basiclistview.h"

#include <QDebug>
#include <QListView>
#include <QVBoxLayout>

using dcc::network::NetworkModel;

VpnControlPage::VpnControlPage(NetworkModel *model, QWidget *parent)
    : QWidget(parent),

      m_listModel(new VpnListModel(model, this))
{
    BasicListView *listView = new BasicListView;

    listView->setModel(m_listModel);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addStretch();
    mainLayout->addWidget(listView);
    mainLayout->setMargin(0);

    setLayout(mainLayout);
}
