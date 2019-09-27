#include "index.h"

#include "modules/sync/syncmodel.h"
#include "modules/sync/syncstateicon.h"

#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "modules/accounts/avatarwidget.h"
#include "downloadurl.h"

#include <DWarningButton>

#include <QScrollArea>
#include <QLabel>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QDebug>
#include <QDateTime>
#include <QMap>
#include <QDir>

DWIDGET_USE_NAMESPACE

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;
using namespace dcc::widgets;
using namespace dcc::cloudsync;
using namespace dcc::accounts;

namespace DCC_NAMESPACE {
namespace sync {

IndexPage::IndexPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout)
    , m_model(nullptr)
    , m_avatar(new AvatarWidget)
    , m_username(new QLabel)
    , m_listView(new DListView)
    , m_stateIcon(new SyncStateIcon)
    , m_lastSyncTimeLbl(new QLabel)
    , m_listModel(new QStandardItemModel)
    , m_downloader(nullptr)
    , m_avatarPath(QString("%1/.cache/deepin/dde-control-center/sync").arg(getenv("HOME")))
{
    //~ contents_path /cloudsync/Auto Sync
    m_autoSyncSwitch = new SwitchWidget(tr("Auto Sync"));

    //~ contents_path /cloudsync/Syncing...
    m_stateLbl = new QLabel(tr("Syncing..."));

    m_stateLbl->hide();
    m_listView->setSelectionMode(QListView::NoSelection);
    m_listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_listView->setEditTriggers(QListView::NoEditTriggers);
    m_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setVerticalScrollMode(QListView::ScrollPerPixel);
    m_listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listView->setFrameShape(QFrame::NoFrame);
    m_listView->setSpacing(5);

    SettingsGroup *autoSyncGrp = new SettingsGroup;
    autoSyncGrp->appendItem(m_autoSyncSwitch);

    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    scrollArea->setContentsMargins(0, 0, 0, 0);

    QWidget *backgroundWidget = new QWidget;
    backgroundWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    backgroundWidget->setFixedWidth(340);
    scrollArea->setFixedWidth(340);
    QVBoxLayout *backgroundLayout = new QVBoxLayout;
    backgroundWidget->setLayout(backgroundLayout);

    scrollArea->setWidget(backgroundWidget);

    backgroundLayout->setMargin(0);
    backgroundLayout->setSpacing(0);

    m_avatar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_avatar->setFixedSize(QSize(100, 100));
    m_username->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    DWarningButton *logoutBtn = new DWarningButton;
    //~ contents_path /cloudsync/Sign Out
    logoutBtn->setText(tr("Sign Out"));

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->setSpacing(0);
    bottomLayout->setMargin(0);
    bottomLayout->setContentsMargins(10, 2, 10, 2);
    bottomLayout->addWidget(m_stateIcon, 0, Qt::AlignLeft);
    bottomLayout->addWidget(m_stateLbl, 0, Qt::AlignCenter);
    bottomLayout->addWidget(m_lastSyncTimeLbl, 0, Qt::AlignCenter);
    bottomLayout->addWidget(logoutBtn, 0, Qt::AlignRight);

    backgroundLayout->addSpacing(20);
    backgroundLayout->addWidget(m_avatar, 0, Qt::AlignHCenter);
    backgroundLayout->addSpacing(14);
    backgroundLayout->addWidget(m_username, 0, Qt::AlignHCenter);
    backgroundLayout->addSpacing(18);
    backgroundLayout->addWidget(autoSyncGrp, 0, Qt::AlignTop);
    backgroundLayout->addSpacing(20);
    backgroundLayout->addWidget(m_listView, 1);


    m_mainLayout->addWidget(scrollArea, 0, Qt::AlignHCenter);
    m_mainLayout->addLayout(bottomLayout, 1);

    m_listView->setModel(m_listModel);

    setLayout(m_mainLayout);

    connect(m_listView, &QListView::clicked, this, &IndexPage::onListViewClicked);
    connect(m_autoSyncSwitch, &SwitchWidget::checkedChanged, this, &IndexPage::requestSetAutoSync);
    connect(logoutBtn, &QPushButton::clicked, this, &IndexPage::requestLogout);
    QDir dir;
    dir.mkpath(m_avatarPath);
}

void IndexPage::setModel(dcc::cloudsync::SyncModel *model)
{
    m_model = model;

    connect(model, &dcc::cloudsync::SyncModel::userInfoChanged, this, &IndexPage::onUserInfoChanged);
    connect(model, &dcc::cloudsync::SyncModel::enableSyncChanged, m_autoSyncSwitch, &SwitchWidget::setChecked);
    connect(model, &dcc::cloudsync::SyncModel::enableSyncChanged, m_listView, &QListView::setVisible);
    connect(model, &dcc::cloudsync::SyncModel::syncStateChanged, this, &IndexPage::onStateChanged);
    connect(model, &dcc::cloudsync::SyncModel::lastSyncTimeChanged, this, &IndexPage::onLastSyncTimeChanged);
    connect(model, &dcc::cloudsync::SyncModel::moduleSyncStateChanged, this, &IndexPage::onModuleStateChanged);

    QMap<SyncType, QPair<QString, QString>> moduleTs{
        //~ contents_path /cloudsync/Network Settings
        { SyncType::Network, {("dcc_sync_internet"), tr("Network Settings") }},
        //~ contents_path /cloudsync/Sound Settings
        { SyncType::Sound, {("dcc_sync_sound"), tr("Sound Settings") }},
        //~ contents_path /cloudsync/Mouse Settings
        { SyncType::Mouse, {("dcc_sync_mouse"), tr("Mouse Settings") }},
        //~ contents_path /cloudsync/Update Settings
        { SyncType::Update, {("dcc_sync_update"), tr("Update Settings") }},
        //~ contents_path /cloudsync/Dock
        { SyncType::Dock, {("dcc_sync_taskbar"), tr("Dock") }},
        //~ contents_path /cloudsync/Launcher
        { SyncType::Launcher, {("dcc_sync_launcher"), tr("Launcher") }},
        //~ contents_path /cloudsync/Wallpaper
        { SyncType::Wallpaper, {("dcc_sync_wallpaper"), tr("Wallpaper") }},
        //~ contents_path /cloudsync/Theme
        { SyncType::Theme, {("dcc_sync_theme"), tr("Theme") }},
        //~ contents_path /cloudsync/Power Settings
        { SyncType::Power, {("dcc_sync_supply"), tr("Power Settings") }},
        //~ contents_path /cloudsync/Corner Settings
        { SyncType::Corner, {("dcc_sync_hot_zone"), tr("Corner Settings") }}
    };

    m_listModel->clear(); // will delete all items

    const std::list<std::pair<SyncType, QStringList>> list = m_model->moduleMap();
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        DStandardItem *item = new DStandardItem;
        item->setCheckable(true);
        item->setIcon(QIcon::fromTheme(moduleTs[it->first].first));
        item->setText(moduleTs[it->first].second);
        item->setData(it->first, Qt::WhatsThisPropertyRole);
        m_listModel->appendRow(item);
        m_itemMap[it->first] = item;
    }

    std::map<SyncType, bool> moduleState = m_model->moduleSyncState().toStdMap();
    for (auto it = moduleState.cbegin(); it != moduleState.cend(); ++it) {
        onModuleStateChanged(*it);
    };

    onUserInfoChanged(model->userinfo());
    m_autoSyncSwitch->setChecked(model->enableSync());
    m_listView->setVisible(model->enableSync());
    onStateChanged(model->syncState());
    onLastSyncTimeChanged(model->lastSyncTime());
}

IndexPage::~IndexPage()
{
    if (m_downloader != nullptr)
        m_downloader->deleteLater();
}

void IndexPage::onListViewClicked(const QModelIndex &index)
{
    QStandardItem *item = (m_itemMap.begin() + index.row()).value();
    const bool enable = item->checkState() == Qt::Checked;
    Q_EMIT requestSetModuleState(std::pair<SyncType, bool>(item->data(Qt::WhatsThisPropertyRole).value<SyncType>()
                                , !enable));
}

void IndexPage::onUserInfoChanged(const QVariantMap &infos)
{
    Q_UNUSED(infos);

    m_username->setText(m_model->userDisplayName());
    QString profile_image = infos.value("profile_image").toString();
    qDebug() << " profile_image = " << profile_image;
    if (profile_image.isEmpty()) //test
        profile_image = "https://avatar.cdn.deepin.com/public/default.png";

    if (m_downloader == nullptr)
        m_downloader = new DownloadUrl;

    m_downloader->downloadFileFromURL(profile_image, m_avatarPath);

    connect(m_downloader, &DownloadUrl::fileDownloaded, this, [this](const QString &fileName) {
        qDebug() << "downloaded filename = " << QUrl::fromLocalFile(fileName).toString();
        m_avatar->setAvatarPath(QUrl::fromLocalFile(fileName).toString());
    });
}

void IndexPage::onStateChanged(const std::pair<qint32, QString> &state)
{
    if (!SyncModel::isSyncStateValid(state)) {
        return;
    }

    SyncState syncState;

    do {
        // check is sync succeed
        if (SyncModel::isSyncSucceed(state)) {
            syncState = SyncState::Succeed;
            break;
        }

        // check is syncing
        if (SyncModel::isSyncing(state)) {
            syncState = SyncState::Syncing;
            break;
        }

        // check is sync faild
        if (SyncModel::isSyncFailed(state)) {
            syncState = SyncState::Failed;
            break;
        }

        Q_UNREACHABLE();

    } while (false);

    switch (syncState) {
    case SyncState::Succeed:
        m_lastSyncTimeLbl->show();
        m_stateLbl->hide();
        m_stateIcon->setRotatePixmap(QIcon::fromTheme("dcc_sync_ok").pixmap(QSize(16, 16)));
        m_stateIcon->stop();
        break;
    case SyncState::Syncing:
        m_lastSyncTimeLbl->hide();
        m_stateLbl->show();
        m_stateIcon->setRotatePixmap(QIcon::fromTheme("dcc_syncing").pixmap(QSize(16, 16)));
        m_stateIcon->play();
        break;
    case SyncState::Failed:
        m_lastSyncTimeLbl->show();
        m_stateLbl->hide();
        m_stateIcon->setRotatePixmap(QPixmap());
        m_stateIcon->stop();
        break;
    }
}

void IndexPage::onLastSyncTimeChanged(const qlonglong lastSyncTime)
{
    m_lastSyncTimeLbl->setText(
        //~ contents_path /cloudsync/Last Sync:
        tr("Last Sync: %1")
        .arg(QDateTime::fromMSecsSinceEpoch(lastSyncTime * 1000)
             .toString(tr("yyyy-MM-dd hh:mm"))));
}

void IndexPage::onModuleStateChanged(std::pair<SyncType, bool> state)
{
    QStandardItem *item = m_itemMap[state.first];
    Q_ASSERT(item);

    item->setCheckState(state.second ? Qt::Checked : Qt::Unchecked);
}

void IndexPage::onAutoSyncChanged(bool autoSync)
{
    m_listView->setVisible(!autoSync);
}

}
}
