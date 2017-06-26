#include "vpncontrolpage.h"
#include "vpnlistdelegate.h"
#include "basiclistview.h"

#include <QDebug>
#include <QListView>
#include <QVBoxLayout>
#include <QEvent>

using dcc::widgets::BasicListView;
using dcc::network::NetworkModel;

VpnControlPage::VpnControlPage(NetworkModel *model, QWidget *parent)
    : QWidget(parent),

      m_listModel(new VpnListModel(model, this))
{
    BasicListView *listView = new BasicListView;
    VpnListDelegate *listDelegate = new VpnListDelegate;

    listView->setModel(m_listModel);
    listView->setItemDelegate(listDelegate);
    listView->installEventFilter(this);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addStretch();
    mainLayout->addWidget(listView);
    mainLayout->setMargin(0);

    setLayout(mainLayout);

    connect(listView, &BasicListView::entered, m_listModel, &VpnListModel::setHoveredIndex);
    connect(listView, &BasicListView::clicked, this, &VpnControlPage::onItemClicked);
}

bool VpnControlPage::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    if (event->type() == QEvent::Leave)
        emit mouseLeaveView();

    return false;
}

void VpnControlPage::onItemClicked(const QModelIndex &index) const
{
    if (index.data(VpnListModel::VpnDisableRole).toBool())
        return;

    if (!index.data(VpnListModel::VpnShowIconRole).toBool())
        emit requestActivateConnection("/", index.data(VpnListModel::VpnUuidRole).toString());
    else
        emit requestDisconnect(index.data(VpnListModel::VpnUuidRole).toString());
}
