#include "index.h"

#include "modules/sync/syncmodel.h"
#include "window/modules/sync/models/synclistmodel.h"

#include <QListView>
#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;

IndexPage::IndexPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout)
    , m_model(nullptr)
    , m_listView(new QListView)
    , m_listModel(new SyncListModel)
{
    m_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setVerticalScrollMode(QListView::ScrollPerPixel);

    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    m_mainLayout->addWidget(m_listView, 0, Qt::AlignHCenter);

    m_listView->setModel(m_listModel);

    setLayout(m_mainLayout);
}

void IndexPage::setModel(dcc::cloudsync::SyncModel *model)
{
    m_model = model;
    m_listModel->setModel(model);
}
