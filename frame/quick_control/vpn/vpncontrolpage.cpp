#include "vpncontrolpage.h"
#include "vpnlistview.h"

#include <QDebug>
#include <QListView>
#include <QVBoxLayout>

using dcc::network::NetworkModel;

VpnControlPage::VpnControlPage(NetworkModel *model, QWidget *parent)
    : QWidget(parent),

      m_listModel(new VpnListModel(model, this))
{
    QListView *listView = new VpnListView;

    listView->setModel(m_listModel);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addStretch();
    mainLayout->addWidget(listView);
    mainLayout->setMargin(0);

    setLayout(mainLayout);

    auto updateViewHeight = [=] { listView->setMaximumHeight(m_listModel->rowCount(QModelIndex()) * listView->sizeHintForRow(0)); };
    connect(m_listModel, &VpnListModel::layoutChanged, updateViewHeight);

    updateViewHeight();
}
