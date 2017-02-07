#include "wifipage.h"
#include "wifilistmodel.h"
#include "../basiclistview.h"

#include <QVBoxLayout>

using dcc::network::NetworkModel;

WifiPage::WifiPage(NetworkModel *model, QWidget *parent)
    : QWidget(parent),

      m_networkModel(model)
{

    WifiListModel *listModel = new WifiListModel(model);
    BasicListView *listView = new BasicListView;
    listView->setModel(listModel);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();
    centralLayout->addWidget(listView);
    centralLayout->setMargin(0);

    setLayout(centralLayout);

    connect(listModel, &WifiListModel::requestDeviceApList, this, &WifiPage::requestDeviceApList);
}
