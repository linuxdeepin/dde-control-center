#include "wifipage.h"
#include "wifilistmodel.h"
#include "wifilistdelegate.h"
#include "basiclistview.h"

#include <QVBoxLayout>
#include <QDebug>

using dcc::network::NetworkModel;
using dcc::widgets::BasicListView;

WifiPage::WifiPage(NetworkModel *model, QWidget *parent)
    : QWidget(parent),

      m_networkModel(model)
{

    WifiListModel *listModel = new WifiListModel(model);
    WifiListDelegate *delegate = new WifiListDelegate;
    BasicListView *listView = new BasicListView;
    listView->setModel(listModel);
    listView->setItemDelegate(delegate);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();
    centralLayout->addWidget(listView);
    centralLayout->setMargin(0);

    setLayout(centralLayout);

    connect(listModel, &WifiListModel::requestDeviceApList, this, &WifiPage::requestDeviceApList);
    connect(listView, &BasicListView::entered, listModel, &WifiListModel::setCurrentHovered);
    connect(listView, &BasicListView::clicked, listModel, &WifiListModel::setCurrentActivating);
    connect(listView, &BasicListView::clicked, this, &WifiPage::onItemClicked);
}

void WifiPage::onItemClicked(const QModelIndex &index)
{
    if (index.data(WifiListModel::ItemIsHeaderRole).toBool())
        return;

    if (index.data(WifiListModel::ItemIsHiddenTipsRole).toBool())
    {
        emit requestConnectHidden();
        return;
    }

    const QString uuid = index.data(WifiListModel::ItemUuidRole).toString();
    if (index.data(WifiListModel::ItemIsActiveRole).toBool())
    {
        emit requestDeactivateConnection(uuid);
    } else {
        const QString devPath = index.data(WifiListModel::ItemDevicePathRole).toString();
        const QString apPath = index.data(WifiListModel::ItemApPathRole).toString();

        emit requestActivateAccessPoint(devPath, apPath, uuid);
    }
}
