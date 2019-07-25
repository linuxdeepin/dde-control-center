#include "index.h"

#include "modules/sync/syncmodel.h"
#include "window/standarditemhelper.h"

#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;

IndexPage::IndexPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout)
    , m_model(nullptr)
    , m_listView(new QListView)
    , m_listModel(new QStandardItemModel)
{
    new StandardItemHelper(m_listView, m_listModel, this);

    m_listView->setSelectionMode(QListView::SingleSelection);
    m_listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_listView->setEditTriggers(QListView::NoEditTriggers);
    m_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setVerticalScrollMode(QListView::ScrollPerPixel);
    m_listView->setSpacing(5);

    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    m_mainLayout->addWidget(m_listView, 0, Qt::AlignHCenter);

    m_listView->setModel(m_listModel);

    setLayout(m_mainLayout);

    connect(m_listView, &QListView::clicked, this, &IndexPage::onListViewClicked);
}

void IndexPage::setModel(dcc::cloudsync::SyncModel *model)
{
    m_model = model;

    const QStringList moduleTs {
        tr("Network Settings"),
        tr("Sound Settings"),
        tr("Mouse Settings"),
        tr("Update Settings"),
        tr("Dock"),
        tr("Launcher"),
        tr("Wallpaper"),
        tr("Theme"),
        tr("Power Settings"),
        tr("Corner Settings"),
    };

    qDeleteAll(m_items);
    m_listModel->clear(); // will delete all items

    auto list = m_model->moduleMap();
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        QStandardItem *item = new QStandardItem;
        item->setCheckable(true);
        item->setText(moduleTs[static_cast<int>(it->first)]);
        m_listModel->appendRow(item);
        m_items << item;
    }
}

void IndexPage::onListViewClicked(const QModelIndex &index)
{
    QStandardItem* item = m_items[index.row()];
    qDebug() << item->text() << item->checkState();
}
