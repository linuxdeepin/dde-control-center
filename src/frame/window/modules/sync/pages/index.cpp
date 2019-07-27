#include "index.h"

#include "modules/sync/syncmodel.h"
#include "window/standarditemhelper.h"

#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"

#include <QScrollArea>
#include <QLabel>
#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;
using namespace dcc::widgets;

IndexPage::IndexPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout)
    , m_model(nullptr)
    , m_avatar(new QLabel)
    , m_username(new QLabel)
    , m_autoSyncSwitch(new SwitchWidget(tr("Auto Sync")))
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
    m_listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listView->setSpacing(5);

    SettingsGroup* autoSyncGrp = new SettingsGroup;
    autoSyncGrp->appendItem(m_autoSyncSwitch);

    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    scrollArea->setContentsMargins(0, 0, 0, 0);

    QWidget* backgroundWidget = new QWidget;
    backgroundWidget->setFixedWidth(340);
    scrollArea->setFixedWidth(340);
    QVBoxLayout* backgroundLayout = new QVBoxLayout;
    backgroundWidget->setLayout(backgroundLayout);

    scrollArea->setWidget(backgroundWidget);

    backgroundLayout->setMargin(0);
    backgroundLayout->setSpacing(0);

    m_avatar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_username->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    backgroundLayout->addWidget(m_avatar, 0, Qt::AlignHCenter);
    backgroundLayout->addWidget(m_username, 0, Qt::AlignHCenter);
    backgroundLayout->addWidget(autoSyncGrp, 0, Qt::AlignTop);
    backgroundLayout->addWidget(m_listView);

    m_mainLayout->addWidget(scrollArea, 0, Qt::AlignHCenter);

    m_listView->setModel(m_listModel);

    setLayout(m_mainLayout);

    connect(m_listView, &QListView::clicked, this, &IndexPage::onListViewClicked);
    connect(m_autoSyncSwitch, &SwitchWidget::checkedChanged, this, &IndexPage::requestSetAutoSync);
}

void IndexPage::setModel(dcc::cloudsync::SyncModel *model)
{
    m_model = model;

    connect(model, &dcc::cloudsync::SyncModel::userInfoChanged, this, &IndexPage::onUserInfoChanged);
    connect(model, &dcc::cloudsync::SyncModel::enableSyncChanged, m_autoSyncSwitch, &SwitchWidget::setChecked);
    connect(model, &dcc::cloudsync::SyncModel::enableSyncChanged, m_listView, &QListView::setVisible);

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

    onUserInfoChanged(model->userinfo());
    m_autoSyncSwitch->setChecked(model->enableSync());
    m_listView->setVisible(model->enableSync());
}

void IndexPage::onListViewClicked(const QModelIndex &index)
{
    QStandardItem* item = m_items[index.row()];
    qDebug() << item->text() << item->checkState();
}

void IndexPage::onUserInfoChanged(const QVariantMap &infos)
{
    Q_UNUSED(infos);

    m_username->setText(m_model->userDisplayName());
}
