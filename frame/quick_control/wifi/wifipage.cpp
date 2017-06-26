#include "wifipage.h"
#include "wifilistmodel.h"
#include "wifilistdelegate.h"
#include "basiclistview.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QEvent>

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
    listView->installEventFilter(this);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();
    centralLayout->addWidget(listView);
    centralLayout->setMargin(0);

    setLayout(centralLayout);

    connect(listModel, &WifiListModel::requestDeviceApList, this, &WifiPage::requestDeviceApList, Qt::QueuedConnection);
    connect(listView, &BasicListView::entered, listModel, &WifiListModel::setCurrentHovered);
    connect(listView, &BasicListView::clicked, listModel, &WifiListModel::setCurrentActivating);
    connect(listView, &BasicListView::clicked, this, &WifiPage::onItemClicked);
}

bool WifiPage::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    if (event->type() == QEvent::Leave)
        emit mouseLeaveView();

    return false;
}

void WifiPage::onItemClicked(const QModelIndex &index)
{
    if (index.data(WifiListModel::ItemIsHeaderRole).toBool() || index.data(WifiListModel::ItemIsPowerOffRole).toBool())
        return;

    if (index.data(WifiListModel::ItemIsHiddenTipsRole).toBool())
    {
        if (index.data(WifiListModel::ItemCountRole) != 1)
            emit requestConnectHidden("network", "");
        else
            emit requestConnectHidden("network", "network");
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
