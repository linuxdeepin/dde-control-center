#include "index.h"

#include "modules/sync/syncmodel.h"
#include "modules/sync/syncstateicon.h"

#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "avatarwidget.h"
#include "downloadurl.h"
#include "window/utils.h"

#include <DWarningButton>
#include <DListView>
#include <DTipLabel>
#include <QDBusPendingReply>
#include <DDBusSender>
#include <DMessageManager>

#include <QScrollArea>
#include <QScrollBar>
#include <QScroller>
#include <QLabel>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QDebug>
#include <QDateTime>
#include <QMap>
#include <QDir>
#include <polkit-qt5-1/PolkitQt1/Authority>

DWIDGET_USE_NAMESPACE

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;
using namespace dcc::widgets;
using namespace dcc::cloudsync;
using namespace PolkitQt1;

namespace DCC_NAMESPACE {
namespace sync {

IndexPage::IndexPage(QWidget *parent)
    : LoginedIn(parent)
    , m_mainLayout(new QVBoxLayout)
    , m_listView(new DListView)
    , m_stateIcon(new SyncStateIcon)
    , m_lastSyncTimeLbl(new QLabel)
    , m_lastSyncTime(0)
    , m_listModel(new QStandardItemModel(this))
{
    m_bindSwitch = new SwitchWidget(tr("Link to the user account"));
    m_bindSwitch->layout()->setContentsMargins(10, 0, 11, 0);
    m_autoSyncSwitch = new SwitchWidget(tr("Auto Sync"));
    m_autoSyncSwitch->layout()->setContentsMargins(10, 6, 11, 6);

    m_lab = new QLabel();
    m_lab->setText(tr("The feature is not available at present, please activate your system first"));
    m_lab->setWordWrap(true);

    m_stateLbl = new QLabel(tr("Syncing..."));

    m_stateLbl->hide();
    m_listView->setAccessibleName("List_indexlist");
    m_listView->setSelectionMode(QListView::NoSelection);
    m_listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_listView->setEditTriggers(QListView::NoEditTriggers);
    m_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setVerticalScrollMode(QListView::ScrollPerPixel);
    m_listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_listView->setFrameShape(QFrame::NoFrame);
    m_listView->setContentsMargins(0, 0, 0, 0);
    m_listView->setIconSize(ListViweIconSize);

    SettingsGroup *autoSyncGrp = new SettingsGroup;
    autoSyncGrp->appendItem(m_bindSwitch);
    autoSyncGrp->appendItem(m_autoSyncSwitch);
    m_autoSyncSwitch->layout()->setContentsMargins(10, 0, 10, 0);
    m_mainLayout->setContentsMargins(8, 8, 8, 8);
    m_mainLayout->setSpacing(0);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setAccessibleName("IndexPage_scrollArea");
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    scrollArea->setContentsMargins(0, 0, 0, 0);
    scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);//暂时没效果,最好还是加上

    QWidget *backgroundWidget = new QWidget;
    backgroundWidget->setAccessibleName("IndexPage_backgroundWidget");
    backgroundWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    backgroundWidget->setFixedWidth(340);
    QVBoxLayout *backgroundLayout = new QVBoxLayout;
    backgroundWidget->setLayout(backgroundLayout);

    //加一个新的布局
    auto hwidget = new QWidget();
    hwidget->setAccessibleName("IndexPage_hwidget");
    auto hBoxlayout = new QHBoxLayout();
    hwidget->setLayout(hBoxlayout);
    hBoxlayout->addWidget(backgroundWidget, 0, Qt::AlignHCenter);

    scrollArea->setWidget(hwidget);

    QScroller::grabGesture(scrollArea->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(hwidget->window());
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    backgroundLayout->setMargin(0);
    backgroundLayout->setSpacing(0);

    DWarningButton *logoutBtn = new DWarningButton;
    logoutBtn->setText(tr("Sign Out"));

    auto bottomWidget = new QWidget();
    bottomWidget->setAccessibleName("IndexPage_bottomWidget");
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->setSpacing(0);
    bottomLayout->setMargin(0);
    bottomLayout->setContentsMargins(10, 2, 10, 2);
    bottomLayout->addWidget(m_stateIcon, 0, Qt::AlignLeft);
    bottomLayout->addWidget(m_stateLbl, 0, Qt::AlignCenter);
    bottomLayout->addWidget(m_lastSyncTimeLbl, 0, Qt::AlignCenter);
    bottomLayout->addWidget(logoutBtn, 0, Qt::AlignRight);
    bottomWidget->setMaximumHeight(120);
    bottomWidget->setLayout(bottomLayout);

    backgroundLayout->addSpacing(141);
    backgroundLayout->addWidget(m_avatar, 0, Qt::AlignHCenter);
    backgroundLayout->addSpacing(14);
    backgroundLayout->addWidget(m_username, 0, Qt::AlignHCenter);
    backgroundLayout->addSpacing(18);
    backgroundLayout->addWidget(autoSyncGrp, 0, Qt::AlignTop);
    backgroundLayout->addSpacing(10);
    backgroundLayout->addWidget(m_lab, 0, Qt::AlignTop);
    backgroundLayout->addSpacing(10);
    backgroundLayout->addWidget(m_listView, 1);
    backgroundLayout->addStretch(1);

    QHBoxLayout *tipLayout = new QHBoxLayout;
    tipLayout->addSpacing(12);
    m_networkTip = new DTipLabel(tr("Your WLAN password will be synced if Network Settings is turned on"));
    m_networkTip->setWordWrap(true);
    tipLayout->addWidget(m_networkTip);
    backgroundLayout->addLayout(tipLayout);

    m_mainLayout->addWidget(scrollArea);
    m_mainLayout->addWidget(bottomWidget);

    m_listView->setModel(m_listModel);

    setLayout(m_mainLayout);

    connect(m_listView, &QListView::clicked, this, &IndexPage::onListViewClicked);
    connect(m_autoSyncSwitch, &SwitchWidget::checkedChanged, this, &IndexPage::requestSetAutoSync);
    connect(m_autoSyncSwitch, &SwitchWidget::checkedChanged, this, &IndexPage::SyncTimeLbl);
    connect(m_bindSwitch, &SwitchWidget::checkedChanged, this, &IndexPage::onBindUserAccountChanged);
    connect(logoutBtn, &QPushButton::clicked, this, [this] {
        Q_EMIT IndexPage::requestLogout();
        if (m_bindSwitch->checked())
            m_bindSwitch->setChecked(!unbindUserAccount());
    });
}

void IndexPage::setModel(dcc::cloudsync::SyncModel *model)
{
    LoginedIn::setModel(model);
    m_lab->setVisible(!model->getActivation());

    connect(model, &dcc::cloudsync::SyncModel::userInfoChanged, this, &IndexPage::onUserInfoChanged);
    connect(model, &dcc::cloudsync::SyncModel::enableSyncChanged, m_autoSyncSwitch, &SwitchWidget::setChecked);
    connect(model, &dcc::cloudsync::SyncModel::enableSyncChanged, m_listView, &QListView::setVisible);
    connect(model, &dcc::cloudsync::SyncModel::enableSyncChanged, m_networkTip, &QLabel::setVisible);
    connect(model, &dcc::cloudsync::SyncModel::enableSyncChanged, this, [=](const bool &value) {
        m_lastSyncTimeLbl->setVisible(value && m_model->lastSyncTime());
    });
    connect(model, &dcc::cloudsync::SyncModel::syncStateChanged, this, &IndexPage::onStateChanged);
    connect(model, &dcc::cloudsync::SyncModel::lastSyncTimeChanged, this, &IndexPage::onLastSyncTimeChanged);
    connect(model, &dcc::cloudsync::SyncModel::moduleSyncStateChanged, this, &IndexPage::onModuleStateChanged);
    connect(model, &dcc::cloudsync::SyncModel::licenseStateChanged, this, [ = ] (const bool &value) {
        m_autoSyncSwitch->setEnabled(value);
        if (m_autoSyncSwitch->checked() && !value) {
            m_autoSyncSwitch->setChecked(false);
            Q_EMIT m_autoSyncSwitch->checkedChanged(m_autoSyncSwitch->checked());
        }
        m_lab->setVisible(!value);
    });

    QMap<SyncType, QPair<QString, QString>> moduleTs{
        { SyncType::Network, {("dcc_sync_internet"), tr("Network Settings") }},
        { SyncType::Sound, {("dcc_sync_sound"), tr("Sound Settings") }},
        { SyncType::Mouse, {("dcc_sync_mouse"), tr("Mouse Settings") }},
        { SyncType::Update, {("dcc_sync_update"), tr("Update Settings") }},
        { SyncType::Dock, {("dcc_sync_taskbar"), tr("Dock") }},
        { SyncType::Launcher, {("dcc_sync_launcher"), tr("Launcher") }},
        { SyncType::Wallpaper, {("dcc_sync_wallpaper"), tr("Wallpaper") }},
        { SyncType::Theme, {("dcc_sync_theme"), tr("Theme") }},
        { SyncType::Power, {("dcc_sync_supply"), tr("Power Settings") }},
    };

    m_listModel->clear(); // will delete all items

    const std::list<std::pair<SyncType, QStringList>> list = m_model->moduleMap();
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        DStandardItem *item = new DStandardItem;
        item->setCheckable(false);
        item->setIcon(QIcon::fromTheme(moduleTs[it->first].first));
        item->setText(moduleTs[it->first].second);
        item->setData(it->first, Qt::WhatsThisPropertyRole);
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_listModel->appendRow(item);
        m_itemMap[it->first] = item;
    }

    std::map<SyncType, bool> moduleState = m_model->moduleSyncState().toStdMap();
    for (auto it = moduleState.cbegin(); it != moduleState.cend(); ++it) {
        onModuleStateChanged(*it);
    };

    onUserInfoChanged(model->userinfo());
    m_autoSyncSwitch->setChecked(model->enableSync() && model->getActivation());
    if (!model->getActivation()) {
        Q_EMIT m_autoSyncSwitch->checkedChanged(m_autoSyncSwitch->checked());
    }
    m_autoSyncSwitch->setEnabled(model->getActivation());
    m_bindSwitch->setChecked(isUserAccountBinded());
    m_networkTip->setVisible(model->enableSync());
    m_listView->setVisible(model->enableSync());
    m_lastSyncTimeLbl->setVisible(model->enableSync() && model->lastSyncTime());
    onStateChanged(model->syncState());
    onLastSyncTimeChanged(model->lastSyncTime());
}

IndexPage::~IndexPage()
{
}

void IndexPage::onListViewClicked(const QModelIndex &index)
{
    QStandardItem *item = (m_itemMap.begin() + index.row()).value();
    const bool enable = item->checkState() == Qt::Checked;
    Q_EMIT requestSetModuleState(std::pair<SyncType, bool>(item->data(Qt::WhatsThisPropertyRole).value<SyncType>()
                                , !enable));
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

    if (!m_autoSyncSwitch->checked()) {
        m_lastSyncTimeLbl->hide();
        m_stateLbl->hide();
        m_stateIcon->setRotatePixmap(QPixmap());
        m_stateIcon->stop();
        return;
    }
    qDebug() << "syncState: " << syncState << "m_lastSyncTime:" << m_lastSyncTime;
    switch (syncState) {
    case SyncState::Succeed:
        m_lastSyncTimeLbl->setVisible(m_model->lastSyncTime() > 0);
        m_stateLbl->hide();
        if (m_model->lastSyncTime() > 0) {
            m_stateIcon->setRotatePixmap(QIcon::fromTheme("dcc_sync_ok").pixmap(QSize(16, 16)));
        } else {
            m_stateIcon->setRotatePixmap(QPixmap());
        }
        m_stateIcon->stop();
        break;
    case SyncState::Syncing:
        m_lastSyncTimeLbl->hide();
        m_stateLbl->show();
        m_stateIcon->setRotatePixmap(QIcon::fromTheme("dcc_syncing").pixmap(QSize(16, 16)));
        m_stateIcon->play();
        break;
    case SyncState::Failed:
        m_lastSyncTimeLbl->setVisible(m_model->lastSyncTime() > 0);
        m_stateLbl->hide();
        m_stateIcon->setRotatePixmap(QPixmap());
        m_stateIcon->stop();
        break;
    }
    m_state = state;
}

void IndexPage::SyncTimeLbl(bool checked)
{
    if (!checked) {
        m_lastSyncTimeLbl->hide();
        m_stateLbl->hide();
        m_stateIcon->setRotatePixmap(QPixmap());
        m_stateIcon->stop();
        return;
    } else if(SyncModel::isSyncStateValid(m_state)) {
        onStateChanged(m_state);
    } else {
        return;
    }
}

void IndexPage::onLastSyncTimeChanged(const qlonglong lastSyncTime)
{
    m_lastSyncTime = lastSyncTime;
    if (lastSyncTime == 0) {
        m_lastSyncTimeLbl->hide();
        return;
    }
    m_lastSyncTimeLbl->setText(
        tr("Last Sync: %1")
        .arg(QDateTime::fromMSecsSinceEpoch(lastSyncTime * 1000)
             .toString(tr("yyyy-MM-dd hh:mm"))));

    onStateChanged(m_state);
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
    m_lastSyncTimeLbl->setVisible(!autoSync);
}

bool IndexPage::isUserAccountBinded()
{
    QString uosid;
    Q_EMIT requestUOSID(uosid);
    if (uosid.isEmpty()) {
        return false;
    }

    QString uuid;
    Q_EMIT requestUUID(uuid);
    if (uuid.isEmpty()) {
        return false;
    }

    QString errorTxt;
    Q_EMIT requestLocalBindCheck(uosid, uuid, m_ubid, errorTxt);
    if (!errorTxt.isEmpty()) {
        QString tips;
        if (errorTxt.contains("7500")) {
            tips = tr("System error");
        } else if (errorTxt.contains("7506")) {
            tips = tr("Network error");
        }
        if (!tips.isEmpty()) {
            DMessageManager::instance()->sendMessage(this,
                                                     style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                     tips);
        }
    }
    if(!m_ubid.isEmpty()) {
        return true;
    } else {
        return false;
    }
}

void IndexPage::onBindUserAccountChanged(bool checked)
{
    Authority::Result authenticationResult;
    authenticationResult = Authority::instance()->checkAuthorizationSync(QString("com.deepin.daemon.accounts.user-administration"),
                                                                         UnixProcessSubject(getpid()),
                                                                         Authority::AllowUserInteraction);

    if (Authority::Result::Yes != authenticationResult) {
        m_bindSwitch->setChecked(!checked);
        return;
    }

    if (checked) {
        m_bindSwitch->setChecked(bindUserAccount());
    } else {
        m_bindSwitch->setChecked(!unbindUserAccount());
    }
}

bool IndexPage::bindUserAccount()
{
    QString uuid;
    Q_EMIT requestUUID(uuid);
    if (uuid.isEmpty()) {
        return false;
    }

    QString hostName;
    Q_EMIT requestHostName(hostName);
    if (hostName.isEmpty()) {
        return false;
    }

    QString errorTxt;
    Q_EMIT requestBindAccount(uuid, hostName, m_ubid, errorTxt);

    if (!errorTxt.isEmpty()) {
        QString tips;
        if (errorTxt.contains("7500")) {
            tips = tr("System error");
        } else if (errorTxt.contains("7506")) {
            tips = tr("Network error");
        } else if (errorTxt.contains("7502")) {
            tips = tr("Login expired, please sign in to the Union ID again");
        }
        if (!tips.isEmpty()) {
            DMessageManager::instance()->sendMessage(this,
                                                     style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                     tips);
        }
    }

    return !m_ubid.isEmpty();
}

bool IndexPage::unbindUserAccount()
{
    if (m_ubid.isEmpty()) {
        qWarning() << "ubid is empty";
        return false;
    }

    bool ret = false;
    QString errorTxt;
    Q_EMIT requestUnBindAccount(m_ubid, ret, errorTxt);
    if (!errorTxt.isEmpty()) {
        QString tips;
        if (errorTxt.contains("7500")) {
            tips = tr("System error");
        } else if (errorTxt.contains("7506")) {
            tips = tr("Network error");
        } else if (errorTxt.contains("7502")) {
            tips = tr("Login expired, please sign in to the Union ID again");
        }
        if (!tips.isEmpty()) {
            DMessageManager::instance()->sendMessage(this,
                                                     style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                     tips);
        }
    }
    return ret;
}

}
}
