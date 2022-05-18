/*
 * Copyright (C) 2016 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "logininfodetail.h"
#include "utils.h"
#include "syncmodel.h"
#include "syncstateicon.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"

#include <polkit-qt5-1/PolkitQt1/Authority>

#include <DTipLabel>
#include <DMessageManager>
#include <DApplicationHelper>
#include <QVBoxLayout>

#include <QObject>
#include <QWidget>
#include <DListView>
#include <QScrollArea>
#include <QScroller>
#include <QPalette>
#include <DSwitchButton>


using namespace PolkitQt1;
DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

LoginInfoDetailPage::LoginInfoDetailPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout(this))
    , m_bindSwitch(new SwitchWidget(tr("Link local account to Union ID"), this))
    , m_autoSyncSwitch(new SwitchWidget(tr("UOS Cloud sync"), this))
    , m_autoSyncTips(new DTipLabel(tr("Store system settings securely in the cloud and keep them in sync across devices"), this))
    , m_bindedTips(new DTipLabel(tr("Linked successfully! Switch it off to get them unlinked."), this))
    , m_listView(new DListView)
    , m_listModel(new QStandardItemModel(this))
    , m_stateIcon(new SyncStateIcon)
    , m_group(new SettingsGroup(nullptr, SettingsGroup::ItemBackground))
    , m_lastSyncTimeLbl(new QLabel(this))
    , m_disabledTips(new DTipLabel(tr("UOS Cloud is currently unavailable in your region"), this))
    , m_syncState(false)
{
    initUI();
    initConnection();
}

LoginInfoDetailPage::~LoginInfoDetailPage()
{
}

void LoginInfoDetailPage::setModel(SyncModel *model)
{
    m_model = model;
    if (!m_model->getActivation())
        showItemDisabledStatus(InfoType::NoActive);

    connect(m_model, &SyncModel::userInfoChanged, this, &LoginInfoDetailPage::onUserInfoChanged);
    connect(m_model, &SyncModel::enableSyncChanged, this, &LoginInfoDetailPage::onAutoSyncChanged);
    connect(m_model, &SyncModel::syncStateChanged, this, &LoginInfoDetailPage::onStateChanged);

    connect(m_model, &SyncModel::lastSyncTimeChanged, this, &LoginInfoDetailPage::onLastSyncTimeChanged);
    connect(m_model, &SyncModel::moduleSyncStateChanged, this, &LoginInfoDetailPage::onModuleStateChanged);
    connect(m_model, &SyncModel::userUnbindInfoChanged, this, &LoginInfoDetailPage::onUserUnbindInfoChanged);
    connect(m_model, &SyncModel::resetPasswdError, this, &LoginInfoDetailPage::onResetPasswdError, Qt::QueuedConnection);
    connect(m_model, &SyncModel::licenseStateChanged, this, [ = ] (const bool &value) {
        if (!value)
            showItemDisabledStatus(InfoType::NoActive);
    });

    m_listModel->clear(); // will delete all items
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

    QSize size(16, 16);
    const std::list<std::pair<SyncType, QStringList>> list = m_model->moduleMap();
    for (auto it = list.cbegin(); it != list.cend(); ++it){
        DStandardItem *item = new DStandardItem;
        SyncType type = it->first;
        QString itemText = moduleTs[it->first].second;
        bool visible = m_model->getModuleStateByType(type);
        QStringList values = it->second;

        item->setIcon(QIcon::fromTheme(moduleTs[it->first].first));
        item->setText(itemText);
        auto rightAction = new DViewItemAction(Qt::AlignVCenter, size, size, true);
        auto checkoutState = visible ? DStyle::SP_IndicatorChecked : DStyle::SP_IndicatorUnchecked;
        auto checkIcon = qobject_cast<DStyle *>(style())->standardIcon(checkoutState);
        rightAction->setIcon(checkIcon);
        item->setActionList(Qt::Edge::RightEdge, {rightAction});

        m_listModel->appendRow(item);
        m_itemMap[it->first] = item;
        connect(rightAction, &DViewItemAction::triggered, this, [ = ] (){
            for (const QString &typeSta : values) {
                Q_EMIT requestSetModuleState(typeSta, !m_model->getModuleStateByType(type));
            }
            updateItemCheckStatus(itemText, !m_model->getModuleStateByType(type));
        });
    }
    m_listView->setModel(m_listModel);

    // TODO: init date
    std::map<SyncType, bool> moduleState = m_model->moduleSyncState().toStdMap();
    for (auto it = moduleState.cbegin(); it != moduleState.cend(); ++it) {
        onModuleStateChanged(*it);
    };
    onUserInfoChanged(model->userinfo());
    m_syncState = model->enableSync() && model->getActivation();
    m_autoSyncSwitch->setChecked(m_syncState);
    if (!model->getActivation()) {
        Q_EMIT m_autoSyncSwitch->checkedChanged(m_autoSyncSwitch->checked());
    }
    m_autoSyncSwitch->setEnabled(model->getActivation());

    m_listView->setVisible(model->enableSync());
    SyncTimeLblVisible(model->enableSync() && model->lastSyncTime());
    onStateChanged(model->syncState());
    onLastSyncTimeChanged(model->lastSyncTime());
    QString bindState = m_model->getUbID();
    onUserUnbindInfoChanged(bindState);
    onAutoSyncChanged(m_syncState);
}

void LoginInfoDetailPage::initUI()
{
    setBackgroundRole(QPalette::Base);
    m_mainLayout->setContentsMargins(8, 8, 8, 8);
    m_mainLayout->setSpacing(0);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    DFontSizeManager::instance()->bind(m_bindSwitch, DFontSizeManager::T6, QFont::DemiBold);
    DFontSizeManager::instance()->bind(m_autoSyncSwitch, DFontSizeManager::T6, QFont::DemiBold);

    // 滑动区： 绑定+同步
    QVBoxLayout *contentLayout = new QVBoxLayout;

    // 绑定
    SettingsItem *bind = new SettingsItem;
    m_bindSwitch->layout()->setContentsMargins(0, 0, 10, 0);
    DTipLabel *bindTips = new DTipLabel(tr("If linked, you can reset passwords of local accounts by Union ID"), this);
    bindTips->setWordWrap(true);
    bindTips->setAlignment(Qt::AlignLeft);
    bindTips->setVisible(true);

    QVBoxLayout *bindLay = new QVBoxLayout;
    bindLay->setContentsMargins(10, 0, 0, 8);
    bindLay->addWidget(m_bindSwitch);
    bindLay->addWidget(bindTips);
    bind->setLayout(bindLay);
    bindLay->setSpacing(0);
    bind->addBackground();

    m_bindedTips->setContentsMargins(10, 0, 10, 0);
    m_bindedTips->setWordWrap(true);
    m_bindedTips->setAlignment(Qt::AlignLeft);
    m_bindedTips->setVisible(false);
    m_bindedTips->setForegroundRole(DPalette::TextTitle);

    // 同步
    m_autoSyncSwitch->layout()->setContentsMargins(0, 0, 0, 0);
    m_autoSyncSwitch->setChecked(false);
    m_autoSyncTips->setWordWrap(true);
    m_autoSyncTips->setAlignment(Qt::AlignLeft);
    m_autoSyncTips->setContentsMargins(0, 0, 10, 0);

    // 同步项
    SettingsItem *head = new SettingsItem;
    QVBoxLayout *syncHeadLay = new QVBoxLayout;
    syncHeadLay->setContentsMargins(10, 0, 10, 8);
    syncHeadLay->addWidget(m_autoSyncSwitch);
    syncHeadLay->addWidget(m_autoSyncTips);
    syncHeadLay->setSpacing(0);
    head->setLayout(syncHeadLay);

    m_listView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_listView->setSelectionMode(QListView::SelectionMode::NoSelection);
    m_listView->setEditTriggers(DListView::NoEditTriggers);
    m_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    m_listView->setFrameShape(DListView::NoFrame);
    m_listView->setViewportMargins(0, 0, 0, 0);
    m_listView->setItemSpacing(1);

    m_listView->setModel(m_listModel);

    m_group->insertItem(0, head);
    m_group->setSpacing(1);
    m_group->insertWidget(m_listView);

    contentLayout->addWidget(bind);
    contentLayout->addWidget(m_bindedTips);
    contentLayout->addSpacing(18);
    contentLayout->addWidget(m_group);

    m_disabledTips->setWordWrap(true);
    m_disabledTips->setAlignment(Qt::AlignLeft);
    m_disabledTips->setContentsMargins(10, 8, 10, 8);
    m_disabledTips->setForegroundRole(DPalette::TextTitle);

    QVBoxLayout *bottomLayout = new QVBoxLayout;
    bottomLayout->setSpacing(0);
    bottomLayout->setMargin(0);
    bottomLayout->setContentsMargins(10, 2, 10, 2);

    QHBoxLayout *syncingLay = new QHBoxLayout;
    syncingLay->setSpacing(0);
    syncingLay->setMargin(0);
    syncingLay->addWidget(m_lastSyncTimeLbl, 0, Qt::AlignLeft);
    syncingLay->addWidget(m_stateIcon, 0, Qt::AlignRight);
    m_stateIcon->setVisible(false);

    bottomLayout->addLayout(syncingLay);
    bottomLayout->addWidget(m_disabledTips, 0, Qt::AlignLeft);
    m_disabledTips->setVisible(false);

    // 保持内容正常铺满
    m_mainLayout->addLayout(contentLayout);
    m_mainLayout->addLayout(bottomLayout);
    m_mainLayout->addStretch();
    setLayout(m_mainLayout);

    if (!IsProfessionalSystem) {
        m_bindSwitch->setChecked(false);
        m_bindSwitch->hide();
    }
}

void LoginInfoDetailPage::initConnection()
{
    connect(m_bindSwitch, &SwitchWidget::checkedChanged, this, &LoginInfoDetailPage::onBindUserAccountChanged);

    connect(m_autoSyncSwitch, &SwitchWidget::checkedChanged, this, &LoginInfoDetailPage::onAutoSyncChanged);
    connect(m_autoSyncSwitch, &SwitchWidget::checkedChanged, this, &LoginInfoDetailPage::requestSetAutoSync);
}

void LoginInfoDetailPage::onUserInfoChanged(const QVariantMap &infos)
{
    // TODO：暂时默认是中国地区
    const QString region = infos["Region"].toString();
    InfoType state = InfoType::Available;
    if (region != "CN" && !region.isEmpty()) {
        state = InfoType::NoService;
    }
    showItemDisabledStatus(state);
    // 登录后监测绑定
    if (!infos["Username"].toString().isEmpty())
        updateUserBindStatus();
}

void LoginInfoDetailPage::updateItemCheckStatus(const QString &name, bool visible)
{
    for (int i = 0; i < m_listModel->rowCount(); ++i) {
        auto item = static_cast<DStandardItem *>(m_listModel->item(i));
        if (item->text() != name || item->actionList(Qt::Edge::RightEdge).size() < 1)
            continue;

        auto action = item->actionList(Qt::Edge::RightEdge).first();
        auto checkstatus = visible ? DStyle::SP_IndicatorChecked : DStyle::SP_IndicatorUnchecked ;
        auto icon = qobject_cast<DStyle *>(style())->standardIcon(checkstatus);
        action->setIcon(icon);
        m_listView->update(item->index());
        break;
    }
}

void LoginInfoDetailPage::onModuleStateChanged(std::pair<SyncType, bool> state)
{
    auto item = static_cast<DStandardItem *>(m_itemMap[state.first]);
    Q_ASSERT(item);

    auto action = item->actionList(Qt::Edge::RightEdge).first();
    auto checkstatus = state.second ? DStyle::SP_IndicatorChecked : DStyle::SP_IndicatorUnchecked;
    auto icon = qobject_cast<DStyle *>(style())->standardIcon(checkstatus);
    action->setIcon(icon);
    m_listView->update(item->index());
}

void LoginInfoDetailPage::onUserUnbindInfoChanged(const QString &ubid)
{
    qDebug() << " bind UBID " << ubid;
    m_ubID = ubid;
    onBindStateChanged(!ubid.isEmpty());
}

void LoginInfoDetailPage::onBindStateChanged(const bool state)
{
    m_bindSwitch->setChecked(state);
    m_bindedTips->setVisible(state);
}

void LoginInfoDetailPage::onStateChanged(const std::pair<qint32, QString> &state)
{
    qDebug() << " SyncState： " << state.first << state.second;
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
    } while (false);

    if (!m_autoSyncSwitch->checked()) {
        m_lastSyncTimeLbl->hide();
        m_stateIcon->setRotatePixmap(QPixmap());
        m_stateIcon->stop();
        return;
    }
    switch (syncState) {
    case SyncState::Succeed:
        SyncTimeLblVisible(m_model->lastSyncTime() > 0);
        if (m_model->lastSyncTime() > 0) {
            m_stateIcon->setRotatePixmap(QIcon::fromTheme("dcc_sync_ok").pixmap(QSize(16, 16)));
        } else {
            m_stateIcon->setRotatePixmap(QPixmap());
        }
        m_stateIcon->stop();
        break;
    case SyncState::Syncing:
        m_lastSyncTimeLbl->hide();
        m_lastSyncTimeLbl->setText(tr("Syncing..."));
        m_stateIcon->setRotatePixmap(QIcon::fromTheme("dcc_syncing").pixmap(QSize(16, 16)));
        m_stateIcon->play();
        break;
    case SyncState::Failed:
        SyncTimeLblVisible(m_model->lastSyncTime() > 0);
        m_stateIcon->setRotatePixmap(QPixmap());
        m_stateIcon->stop();
        break;
    }
}

void LoginInfoDetailPage::onLastSyncTimeChanged(const qlonglong lastSyncTime)
{
    if (lastSyncTime == 0) {
        m_lastSyncTimeLbl->hide();
        return;
    }
    m_lastSyncTimeLbl->setText(
        tr("Last Sync: %1")
        .arg(QDateTime::fromMSecsSinceEpoch(lastSyncTime * 1000)
             .toString(tr("yyyy-MM-dd hh:mm"))));
    SyncTimeLblVisible(true);
}

void LoginInfoDetailPage::onAutoSyncChanged(const bool state)
{
    m_syncState = state;
    m_autoSyncSwitch->setChecked(state);
    m_listView->setVisible(state);
    SyncTimeLblVisible(state && m_model->lastSyncTime());
}

void LoginInfoDetailPage::onResetPasswdError(const QString &error)
{
    qDebug() << "ResetPasswd error: " << error;
    if (!error.isEmpty()) {
        QString tips;
        if (error.contains("7500")) {
            tips = tr("System error");
        } else if (error.contains("7506")) {
            tips = tr("Network error");
        } else if (error.contains("7502")) {
            tips = tr("Login expired, please sign in to the Union ID again");
        }

        if (!tips.isEmpty()) {
            DMessageManager::instance()->sendMessage(this,
                                                     style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                     tips);
        }
    }
}

void LoginInfoDetailPage::updateUserBindStatus()
{
    // 用于监测用户登录后的 绑定登录状态
    QString uuid = m_model->getUUID();
    qDebug() << " updateUserBindInfo : " << uuid;
    if (uuid.isEmpty()) {
        return;
    }

    Q_EMIT requestLocalBindCheck(uuid);
}

void LoginInfoDetailPage::onBindUserAccountChanged(bool checked)
{
    Authority::Result authenticationResult;
    authenticationResult = Authority::instance()->checkAuthorizationSync(QString("com.deepin.daemon.accounts.user-administration"),
                                                                         UnixProcessSubject(getpid()),
                                                                         Authority::AllowUserInteraction);

    if (Authority::Result::Yes != authenticationResult) {
        m_bindSwitch->setChecked(!checked);
        return;
    }
    m_bindSwitch->setChecked(!checked);
    if (checked) {
        bindUserAccount();
    } else {
        unbindUserAccount();
    }
}

void LoginInfoDetailPage::bindUserAccount()
{
    QString uuid = m_model->getUUID();
    QString hostName = m_model->getHostName();
    if (uuid.isEmpty() || hostName.isEmpty())
        return;

    Q_EMIT requestBindAccount(uuid, hostName);
}

void LoginInfoDetailPage::unbindUserAccount()
{
    if (m_ubID.isEmpty()) {
        qWarning() << "ubid is empty";
        return;
    }

    Q_EMIT requestUnBindAccount(m_ubID);
}

void LoginInfoDetailPage::showItemDisabledStatus(LoginInfoDetailPage::InfoType status)
{
    if (status == InfoType::Available) {
        m_bindSwitch->switchButton()->setEnabled(true);
        m_autoSyncSwitch->switchButton()->setEnabled(true);
        onAutoSyncChanged(m_syncState);

        m_disabledTips->setVisible(false);
    }

    if (status == InfoType::NoService) {
        m_bindSwitch->setChecked(false);
        m_bindSwitch->switchButton()->setEnabled(false);
        m_autoSyncSwitch->setChecked(false);
        m_autoSyncSwitch->switchButton()->setEnabled(false);

        m_listView->setVisible(false);
        SyncTimeLblVisible(false);

        m_disabledTips->setVisible(true);
        m_disabledTips->setText(tr("UOS Cloud is currently unavailable in your region"));
    }

    if (status == InfoType::NoActive) {
        m_bindSwitch->setChecked(false);
        m_bindSwitch->switchButton()->setEnabled(false);
        m_autoSyncSwitch->setChecked(false);
        m_autoSyncSwitch->switchButton()->setEnabled(false);

        m_bindSwitch->setEnabled(false);
        m_autoSyncSwitch->setEnabled(false);
        m_disabledTips->setText(tr("The feature is not available at present, please activate your system first"));
    }
}

void LoginInfoDetailPage::SyncTimeLblVisible(bool isVisible)
{
    if (!m_autoSyncSwitch->checked() || !m_autoSyncSwitch->switchButton()->isEnabled()) {
        m_lastSyncTimeLbl->setVisible(false);
        return;
    }
    m_lastSyncTimeLbl->setVisible(isVisible);
}
