/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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

#include "updatectrlwidget.h"
#include "modules/update/updateitem.h"
#include "widgets/translucentframe.h"
#include "modules/update/updatemodel.h"
#include "loadingitem.h"
#include "widgets/labels/normallabel.h"
#include "widgets/settingsgroup.h"
#include "modules/update/summaryitem.h"
#include "modules/update/downloadprogressbar.h"
#include "modules/update/resultitem.h"
#include "widgets/labels/tipslabel.h"

#include <types/appupdateinfolist.h>
#include <QVBoxLayout>
#include <QSettings>
#include <QPushButton>
#include <QScrollArea>
#include <DFontSizeManager>
#include <DPalette>
#include <DSysInfo>

#define UpgradeWarningSize 500

using namespace dcc;
using namespace dcc::update;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::update;

UpdateCtrlWidget::UpdateCtrlWidget(UpdateModel *model, QWidget *parent)
    : QWidget(parent)
    , m_model(nullptr)
    , m_status(UpdatesStatus::Updated)
    , m_checkUpdateItem(new LoadingItem())
    , m_resultItem(new ResultItem())
    , m_progress(new DownloadProgressBar(parent))
    , m_fullProcess(new DownloadProgressBar(parent))
    , m_upgradeWarningGroup(new SettingsGroup)
    , m_summary(new SummaryItem)
    , m_upgradeWarning(new SummaryItem)
    , m_powerTip(new TipsLabel)
    , m_reminderTip(new TipsLabel)
    , m_noNetworkTip(new TipsLabel)
    , m_qsettings(new QSettings(this))
    , m_bRecoverBackingUp(false)
    , m_bRecoverConfigValid(false)
    , m_bRecoverRestoring(false)
    , m_activeState(UiActiveState::Unknown)
    , m_updateList(new ContentWidget(parent))
    , m_authorizationPrompt(new TipsLabel(parent))
    , m_isUpdateingAll(false)
    , m_checkUpdateBtn(new QPushButton(parent))
    , m_lastCheckTimeTip(new TipsLabel(parent))
    , m_CheckAgainBtn(new QPushButton(tr("Check Again")))
    , m_lastCheckAgainTimeTip(new TipsLabel(parent))
    , m_versrionTip(new DLabel(parent))
    , m_spinner(new DSpinner(parent))
    , m_updateTipsLab(new DLabel(parent))
    , m_updateSizeLab(new DLabel(parent))
    , m_updateingTipsLab(new DLabel(parent))
    , m_fullUpdateBtn(new QPushButton)
    , m_updateSize(0)
    , m_systemUpdateItem(new SystemUpdateItem(parent))
    , m_storeUpdateItem(new AppstoreUpdateItem(parent))
    , m_safeUpdateItem(new SafeUpdateItem(parent))
    , m_unknownUpdateItem(new UnknownUpdateItem(parent))
    , m_updateSummaryGroup(new SettingsGroup)
{
    setAccessibleName("UpdateCtrlWidget");
    m_checkUpdateItem->setAccessibleName("checkUpdateItem");

    m_updateList->setAccessibleName("updateList");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_reminderTip->setText(tr("Restart the computer to use the system and the applications properly"));
    m_noNetworkTip->setText(tr("Network disconnected, please retry after connected"));

    m_progress->setVisible(false);

    QVBoxLayout *fullProcesslayout = new QVBoxLayout();

    m_fullProcess->setVisible(false);
    m_fullProcess->setProcessValue(100);

    m_authorizationPrompt->setText(tr("Your system is not authorized, please activate first"));
    m_authorizationPrompt->setAlignment(Qt::AlignHCenter);
    m_authorizationPrompt->setVisible(false);
    fullProcesslayout->addWidget(m_fullProcess);
    fullProcesslayout->addWidget(m_authorizationPrompt);

    m_powerTip->setWordWrap(true);
    m_powerTip->setAlignment(Qt::AlignHCenter);
    m_powerTip->setVisible(false);

    m_reminderTip->setWordWrap(true);
    m_reminderTip->setAlignment(Qt::AlignHCenter);
    m_reminderTip->setVisible(false);

    m_noNetworkTip->setWordWrap(true);
    m_noNetworkTip->setAlignment(Qt::AlignHCenter);
    m_noNetworkTip->setVisible(false);

    m_upgradeWarning->setTitle(tr("This update may take a long time, please do not shut down or reboot during the process"));
    m_upgradeWarning->setContentsMargins(20, 0, 20, 0);
    m_upgradeWarningGroup->setVisible(false);
    m_upgradeWarningGroup->appendItem(m_upgradeWarning);

    m_checkUpdateBtn->setFixedSize(QSize(300, 36));
    m_checkUpdateBtn->setVisible(false);
    m_lastCheckTimeTip->setAlignment(Qt::AlignCenter);
    m_lastCheckTimeTip->setVisible(false);

    QHBoxLayout *updateTitleHLay = new QHBoxLayout;
    QVBoxLayout *updateTitleFirstVLay = new QVBoxLayout;

    m_updateTipsLab->setText(tr("Updates Available"));
    DFontSizeManager::instance()->bind(m_updateTipsLab, DFontSizeManager::T5, QFont::DemiBold);
    m_updateTipsLab->setForegroundRole(DPalette::TextTitle);
    m_updateTipsLab->setVisible(false);

    DFontSizeManager::instance()->bind(m_versrionTip, DFontSizeManager::T8);
    m_versrionTip->setForegroundRole(DPalette::TextTips);    
    QString sVersion = QString("%1 %2").arg(Dtk::Core::DSysInfo::uosProductTypeName()).arg(Dtk::Core::DSysInfo::minorVersion());
    m_versrionTip->setText(tr("Current Edition") + "：" + sVersion);

    updateTitleFirstVLay->addWidget(m_updateTipsLab);
    updateTitleFirstVLay->addWidget(m_updateSizeLab);

    updateTitleHLay->setContentsMargins(QMargins(12, 50, 10, 20));
    updateTitleHLay->addLayout(updateTitleFirstVLay);
    updateTitleHLay->addWidget(m_spinner, 1, Qt::AlignRight);
    m_spinner->setVisible(false);
    m_spinner->setFixedSize(24, 24);
    m_updateingTipsLab->setText(tr("Updating..."));
    m_updateingTipsLab->setVisible(false);
    DFontSizeManager::instance()->bind(m_updateingTipsLab, DFontSizeManager::T8);
    m_updateingTipsLab->setForegroundRole(DPalette::TextTips);
    updateTitleHLay->addSpacing(5);
    updateTitleHLay->addWidget(m_updateingTipsLab);
    m_fullUpdateBtn->setText(tr("Update All"));
    m_fullUpdateBtn->setFixedSize(92, 36);
    m_fullUpdateBtn->setVisible(false);
    updateTitleHLay->addWidget(m_fullUpdateBtn);


    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_versrionTip, 0, Qt::AlignHCenter);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    layout->addWidget(m_upgradeWarningGroup);
    layout->addWidget(m_powerTip);
    layout->addWidget(m_summary);
    layout->addWidget(m_progress);
    layout->addLayout(fullProcesslayout);

    layout->addLayout(updateTitleHLay);
    layout->addWidget(m_updateList, 1);

    layout->addStretch();
    layout->addWidget(m_resultItem);
    layout->addWidget(m_checkUpdateItem);
    layout->addWidget(m_reminderTip);
    layout->addWidget(m_noNetworkTip);
    layout->addSpacing(20);
    layout->addWidget(m_checkUpdateBtn, 0, Qt::AlignCenter);
    layout->addSpacing(5);
    layout->addWidget(m_lastCheckTimeTip);
    layout->addStretch();

    setLayout(layout);

    QWidget *contentWidget = new QWidget;
    contentWidget->setAccessibleName("UpdateCtrlWidget_contentWidget");
    QVBoxLayout *contentLayout = new QVBoxLayout;

    m_systemUpdateItem->setVisible(false);
    m_updateSummaryGroup->appendItem(m_systemUpdateItem);

    m_storeUpdateItem->setVisible(false);
    m_updateSummaryGroup->appendItem(m_storeUpdateItem);

    m_safeUpdateItem->setVisible(false);
    m_updateSummaryGroup->appendItem(m_safeUpdateItem);

    m_unknownUpdateItem->setVisible(false);
    m_updateSummaryGroup->appendItem(m_unknownUpdateItem);

    m_updateSummaryGroup->setVisible(false);
    contentLayout->addWidget(m_updateSummaryGroup);
    contentLayout->addStretch();

    m_CheckAgainBtn->setVisible(false);
    m_lastCheckAgainTimeTip->setVisible(false);
    m_CheckAgainBtn->setFixedSize(QSize(300, 36));
    m_lastCheckAgainTimeTip->setAlignment(Qt::AlignCenter);

    contentLayout->addSpacing(20);
    contentLayout->addWidget(m_CheckAgainBtn, 0, Qt::AlignCenter);
    contentLayout->addSpacing(5);
    contentLayout->addWidget(m_lastCheckAgainTimeTip);
    contentWidget->setLayout(contentLayout);

    m_updateList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_updateList->setContent(contentWidget);

    setModel(model);

    initConnect();
}


void UpdateCtrlWidget::initConnect()
{
    auto initUpdateItemConnect = [ = ](UpdateSettingItem * updateItem) {
        connect(updateItem, &UpdateSettingItem::requestUpdate, this, &UpdateCtrlWidget::onRequestUpdate);
        connect(updateItem, &UpdateSettingItem::requestUpdateCtrl, this, &UpdateCtrlWidget::requestUpdateCtrl);
        connect(updateItem, &UpdateSettingItem::requestRefreshSize, this, &UpdateCtrlWidget::onRequestRefreshSize);
    };

    initUpdateItemConnect(m_systemUpdateItem);
    initUpdateItemConnect(m_storeUpdateItem);
    initUpdateItemConnect(m_safeUpdateItem);
    initUpdateItemConnect(m_unknownUpdateItem);

    connect(m_storeUpdateItem, &AppstoreUpdateItem::requestOpenAppStroe, this, &UpdateCtrlWidget::requestOpenAppStroe);

    connect(m_fullUpdateBtn, &QPushButton::clicked, this, &UpdateCtrlWidget::onFullUpdateClicked);
    connect(m_checkUpdateBtn, &QPushButton::clicked, m_model, &UpdateModel::beginCheckUpdate);
    connect(m_checkUpdateBtn, &QPushButton::clicked, [this] {
        this->setFocus();
        m_isUpdateingAll = false;
    });
    connect(m_CheckAgainBtn, &QPushButton::clicked, m_model, &UpdateModel::beginCheckUpdate);
    connect(m_CheckAgainBtn, &QPushButton::clicked, [this] {
        this->setFocus();
        m_isUpdateingAll = false;
    });
}

UpdateCtrlWidget::~UpdateCtrlWidget()
{
}

void UpdateCtrlWidget::setShowInfo(const UiActiveState value)
{
    bool activation;
    if (UiActiveState::Authorized == value || UiActiveState::TrialAuthorized == value || UiActiveState::AuthorizedLapse == value) {
        activation = true;
    } else {
        activation = false;
    }

    m_fullProcess->setEnabled(activation);
    m_authorizationPrompt->setVisible(UpdatesStatus::UpdatesAvailable == m_model->status() && !activation);
}

void UpdateCtrlWidget::setStatus(const UpdatesStatus &status)
{
    m_status = status;

    if (m_model->systemActivation() == UiActiveState::Unauthorized || m_model->systemActivation() == UiActiveState::TrialExpired) {
        m_status = NoAtive;
    }

    Q_EMIT notifyUpdateState(m_status);

    m_powerTip->setVisible(false);
    m_noNetworkTip->setVisible(false);
    m_resultItem->setVisible(false);
    m_progress->setVisible(false);
    m_fullProcess->setVisible(false);
    m_authorizationPrompt->setVisible(false);
    m_updateList->setVisible(false);
    m_upgradeWarningGroup->setVisible(false);
    m_reminderTip->setVisible(false);
    m_checkUpdateItem->setVisible(false);
    m_checkUpdateItem->setProgressBarVisible(false);
    m_checkUpdateItem->setImageAndTextVisible(false);
    m_summary->setVisible(false);
    m_checkUpdateBtn->setVisible(false);
    m_lastCheckTimeTip->setVisible(false);
    m_updateSummaryGroup->setVisible(false);

    m_CheckAgainBtn->setVisible(false);
    m_lastCheckAgainTimeTip->setVisible(false);
    m_versrionTip->setVisible(false);
    m_updateTipsLab->setVisible(false);
    m_updateSizeLab->setVisible(false);
    m_fullUpdateBtn->setVisible(false);
    m_spinner->setVisible(false);
    m_updateingTipsLab->setVisible(false);

    auto showCheckButton = [this](const QString & caption) {
        m_model->updateCheckUpdateTime();
        m_checkUpdateBtn->setText(caption);
        m_checkUpdateBtn->setVisible(true);
        m_lastCheckTimeTip->setText(tr("Last checking time: ") + m_model->lastCheckUpdateTime());
        m_lastCheckTimeTip->setVisible(true);
    };

    switch (m_status) {
    case UpdatesStatus::Default:
        m_checkUpdateItem->setVisible(true);
        m_checkUpdateItem->setMessage(tr("Your system is up to date"));
        m_checkUpdateItem->setImageOrTextVisible(true);
        m_checkUpdateItem->setSystemVersion(m_systemVersion);
        showCheckButton(tr("Check for Updates"));
        break;
    case UpdatesStatus::NoAtive:
        m_resultItem->setVisible(true);
        m_resultItem->setSuccess(ShowStatus::NoActive);
        break;
    case UpdatesStatus::Checking:
        m_model->beginCheckUpdate();
        m_checkUpdateItem->setVisible(true);
        m_checkUpdateItem->setProgressBarVisible(true);
        m_checkUpdateItem->setMessage(tr("Checking for updates, please wait..."));
        m_checkUpdateItem->setImageOrTextVisible(false);
        break;
    case UpdatesStatus::UpdatesAvailable:
        onChangeUpdatesAvailableStatus();
        break;
    case UpdatesStatus::Updateing:
        showUpdateInfo();
        onRequestRefreshSize();
        break;
    case UpdatesStatus::UpdateSucceeded:
        m_resultItem->setVisible(true);
        m_resultItem->setSuccess(ShowStatus::IsSuccessed);
        m_reminderTip->setVisible(true);
        break;
    case UpdatesStatus::UpdateFailed:
        m_resultItem->setVisible(true);
        m_resultItem->setSuccess(ShowStatus::IsFailed);
        showCheckButton(tr("Check Again"));
        break;
    case UpdatesStatus::RecoveryBackupFailed:
        showUpdateInfo();
        m_CheckAgainBtn->setEnabled(false);
        break;
    case UpdatesStatus::Updated:
        m_checkUpdateItem->setVisible(true);
        m_checkUpdateItem->setMessage(tr("Your system is up to date"));
        m_checkUpdateItem->setImageOrTextVisible(true);
        m_checkUpdateItem->setSystemVersion(m_systemVersion);
        showCheckButton(tr("Check for Updates"));
        break;
    case UpdatesStatus::NeedRestart:
        m_checkUpdateItem->setVisible(true);
        m_checkUpdateItem->setMessage(tr("The newest system installed, restart to take effect"));
        break;
    case UpdatesStatus::NoNetwork:
        m_resultItem->setVisible(true);
        m_resultItem->setSuccess(ShowStatus::IsFailed);
        m_noNetworkTip->setVisible(true);
        showCheckButton(tr("Check Again"));
        break;
    case UpdatesStatus::NoSpace:
        m_resultItem->setVisible(true);
        m_resultItem->setSuccess(ShowStatus::IsFailed);
        m_resultItem->setMessage(tr("Update failed: insufficient disk space"));
        showCheckButton(tr("Check Again"));
        break;
    case UpdatesStatus::DeependenciesBrokenError:
        m_resultItem->setVisible(true);
        m_resultItem->setSuccess(ShowStatus::IsFailed);
        m_resultItem->setMessage(tr("Dependency error, failed to detect the updates"));
        showCheckButton(tr("Check Again"));
        break;
    default:
        qDebug() << "unknown status!!!";
        break;
    }
}

void UpdateCtrlWidget::setSystemUpdateStatus(const UpdatesStatus &status)
{

    m_systemUpdateItem->setStatus(status);

}

void UpdateCtrlWidget::setAppUpdateStatus(const UpdatesStatus &status)
{
    m_storeUpdateItem->setStatus(status);
}

void UpdateCtrlWidget::setSafeUpdateStatus(const UpdatesStatus &status)
{

    m_safeUpdateItem->setStatus(status);
}

void UpdateCtrlWidget::setUnkonowUpdateStatus(const UpdatesStatus &status)
{
    m_unknownUpdateItem->setStatus(status);
}

void UpdateCtrlWidget::setProgressValue(const double value)
{
    m_progress->setProcessValue(static_cast<int>(value * 100));

    if (m_status == UpdatesStatus::Downloading) {
        m_progress->setMessage(tr("%1% downloaded (Click to pause)").arg(qFloor(value * 100)));
    } else if (m_status == UpdatesStatus::DownloadPaused) {
        m_progress->setMessage(tr("%1% downloaded (Click to continue)").arg(qFloor(value * 100)));
    }
}

void UpdateCtrlWidget::setLowBattery(const bool &lowBattery)
{
    if (m_status == UpdatesStatus::Updateing || m_status == UpdatesStatus::UpdatesAvailable) {
        bool activation = false;
        const UiActiveState value = m_model->systemActivation();
        if (UiActiveState::Authorized == value || UiActiveState::TrialAuthorized == value || UiActiveState::AuthorizedLapse == value) {
            activation = true;
        }
        if (lowBattery) {
            m_powerTip->setText(tr("Your battery is lower than 50%, please plug in to continue"));
        } else {
            m_powerTip->setText(tr("Please ensure sufficient power to restart, and don't power off or unplug your machine"));
        }
        //电量和授权共同决定
        bool enable = false;
        if(lowBattery)
            enable = !lowBattery;
        else
            enable = activation;

        m_systemUpdateItem->setLowBattery(enable);
        m_storeUpdateItem->setLowBattery(enable);
        m_safeUpdateItem->setLowBattery(enable);
        m_unknownUpdateItem->setLowBattery(enable);
        m_fullUpdateBtn->setEnabled(enable);

        m_powerTip->setVisible(lowBattery);
    }
}

void UpdateCtrlWidget::setUpdateProgress(const double value)
{
    m_checkUpdateItem->setProgressValue(static_cast<int>(value * 100));
}

void UpdateCtrlWidget::setRecoverBackingUp(const bool value)
{
    if (m_bRecoverBackingUp != value) {
        m_bRecoverBackingUp = value;
    }
}

void UpdateCtrlWidget::setRecoverConfigValid(const bool value)
{
    if (m_bRecoverConfigValid != value) {
        m_bRecoverConfigValid = value;
    }
}

void UpdateCtrlWidget::setRecoverRestoring(const bool value)
{
    if (m_bRecoverRestoring != value) {
        m_bRecoverRestoring = value;
    }
}

void UpdateCtrlWidget::setActiveState(const UiActiveState &activestate)
{
    if (m_activeState != activestate) {
        m_activeState = activestate;
    }

    if (m_model->enterCheckUpdate()) {
        setStatus(UpdatesStatus::Checking);
    } else {
        setStatus(m_model->status());
    }
}


void UpdateCtrlWidget::setModel(UpdateModel *model)
{
    m_model = model;

    connect(m_model, &UpdateModel::statusChanged, this, &UpdateCtrlWidget::setStatus);

    connect(m_model, &UpdateModel::systemUpdateStatusChanged, m_systemUpdateItem, &UpdateSettingItem::onUpdateStatuChanged);
    connect(m_model, &UpdateModel::appUpdateStatusChanged, m_storeUpdateItem, &UpdateSettingItem::onUpdateStatuChanged);
    connect(m_model, &UpdateModel::safeUpdateStatusChanged, m_safeUpdateItem, &UpdateSettingItem::onUpdateStatuChanged);
    connect(m_model, &UpdateModel::unkonowUpdateStatusChanged, m_unknownUpdateItem, &UpdateSettingItem::onUpdateStatuChanged);

    connect(m_model, &UpdateModel::lowBatteryChanged, this, &UpdateCtrlWidget::setLowBattery);

    connect(m_model, &UpdateModel::upgradeProgressChanged, this, &UpdateCtrlWidget::setProgressValue);
    connect(m_model, &UpdateModel::updateProgressChanged, this, &UpdateCtrlWidget::setUpdateProgress);
    connect(m_model, &UpdateModel::recoverBackingUpChanged, this, &UpdateCtrlWidget::setRecoverBackingUp);
    connect(m_model, &UpdateModel::recoverConfigValidChanged, this, &UpdateCtrlWidget::setRecoverConfigValid);
    connect(m_model, &UpdateModel::recoverRestoringChanged, this, &UpdateCtrlWidget::setRecoverRestoring);
    connect(m_model, &UpdateModel::systemActivationChanged, this, &UpdateCtrlWidget::setActiveState);

    connect(m_model, &UpdateModel::systemUpdateInfoChanged, this, &UpdateCtrlWidget::setSystemUpdateInfo);
    connect(m_model, &UpdateModel::appUpdateInfoChanged, this, &UpdateCtrlWidget::setAppUpdateInfo);
    connect(m_model, &UpdateModel::safeUpdateInfoChanged, this, &UpdateCtrlWidget::setSafeUpdateInfo);
    connect(m_model, &UpdateModel::unknownUpdateInfoChanged, this, &UpdateCtrlWidget::setUnkonowUpdateInfo);

    connect(m_model, &UpdateModel::systemUpdateProgressChanged, m_systemUpdateItem, &UpdateSettingItem::onUpdateProgressChanged);
    connect(m_model, &UpdateModel::appUpdateProgressChanged, m_storeUpdateItem, &UpdateSettingItem::onUpdateProgressChanged);
    connect(m_model, &UpdateModel::safeUpdateProgressChanged, m_safeUpdateItem, &UpdateSettingItem::onUpdateProgressChanged);
    connect(m_model, &UpdateModel::unkonowUpdateProgressChanged, m_unknownUpdateItem, &UpdateSettingItem::onUpdateProgressChanged);

    setUpdateProgress(m_model->updateProgress());
    setProgressValue(m_model->upgradeProgress());

    setSystemUpdateInfo(m_model->systemDownloadInfo());
    setAppUpdateInfo(m_model->appDownloadInfo());
    setSafeUpdateInfo(m_model->safeDownloadInfo());
    setUnkonowUpdateInfo(m_model->unknownDownloadInfo());

    if (m_model->enterCheckUpdate()) {
        setStatus(UpdatesStatus::Checking);
    } else {
        setStatus(m_model->status());
        setSystemUpdateStatus(m_model->getSystemUpdateStatus());
        setAppUpdateStatus(m_model->getAppUpdateStatus());
        setSafeUpdateStatus(m_model->getSafeUpdateStatus());
        setUnkonowUpdateStatus(m_model->getUnkonowUpdateStatus());
    }

    setLowBattery(m_model->lowBattery());
}

void UpdateCtrlWidget::setSystemVersion(const QString &version)
{
    if (m_systemVersion != version) {
        m_systemVersion = version;
    }
}

void UpdateCtrlWidget::setSystemUpdateInfo(UpdateItemInfo *updateItemInfo)
{
    if (nullptr == updateItemInfo) {
        m_systemUpdateItem->setVisible(false);
        return;
    }

    initUpdateItem(m_systemUpdateItem);
    m_systemUpdateItem->setData(updateItemInfo);
}

void UpdateCtrlWidget::setAppUpdateInfo(UpdateItemInfo *updateItemInfo)
{

    if (nullptr == updateItemInfo) {
        m_storeUpdateItem->setVisible(false);
        return;
    }

    initUpdateItem(m_storeUpdateItem);
    m_storeUpdateItem->setData(updateItemInfo);

}

void UpdateCtrlWidget::setSafeUpdateInfo(UpdateItemInfo *updateItemInfo)
{
    if (nullptr == updateItemInfo) {
        m_safeUpdateItem->setVisible(false);
        return;
    }

    initUpdateItem(m_safeUpdateItem);
    m_safeUpdateItem->setData(updateItemInfo);


}

void UpdateCtrlWidget::setUnkonowUpdateInfo(UpdateItemInfo *updateItemInfo)
{
    if (nullptr == updateItemInfo) {
        m_unknownUpdateItem->setVisible(false);
        return;
    }

    initUpdateItem(m_unknownUpdateItem);
    m_unknownUpdateItem->setData(updateItemInfo);

}

void UpdateCtrlWidget::setAllUpdateInfo(QMap<ClassifyUpdateType, UpdateItemInfo *> updateInfoMap)
{
    setSystemUpdateInfo(updateInfoMap.value(ClassifyUpdateType::SystemUpdate));
    setAppUpdateInfo(updateInfoMap.value(ClassifyUpdateType::AppStoreUpdate));
    setSafeUpdateInfo(updateInfoMap.value(ClassifyUpdateType::SecurityUpdate));
    setUnkonowUpdateInfo(updateInfoMap.value(ClassifyUpdateType::UnknownUpdate));
}

void UpdateCtrlWidget::showUpdateInfo()
{
    m_fullProcess->setVisible(false);
    m_updateList->setVisible(true);
    m_summary->setVisible(false);
    m_powerTip->setVisible(false);

    if (!m_isUpdateingAll) {
        m_fullUpdateBtn->setVisible(true);
    } else {
        m_spinner->setVisible(true);
        m_spinner->start();
        m_updateingTipsLab->setVisible(true);
        m_fullUpdateBtn->setVisible(false);
    }

    m_updateTipsLab->setVisible(true);
    m_updateSizeLab->setVisible(true);
    m_versrionTip->setVisible(true);

    m_CheckAgainBtn->setVisible(true);
    m_model->updateCheckUpdateTime();
    m_lastCheckAgainTimeTip->setText(tr("Last checking time: ") + m_model->lastCheckUpdateTime());
    m_lastCheckAgainTimeTip->setVisible(true);
    m_updateSummaryGroup->setVisible(true);

    setSystemUpdateStatus(m_model->getSystemUpdateStatus());
    setAppUpdateStatus(m_model->getAppUpdateStatus());
    setSafeUpdateStatus(m_model->getSafeUpdateStatus());
    setUnkonowUpdateStatus(m_model->getUnkonowUpdateStatus());

    if(m_systemUpdateItem->status() == UpdatesStatus::Default || m_systemUpdateItem->status() == UpdatesStatus::UpdateSucceeded){
        m_systemUpdateItem->setVisible(false);
    }

    if(m_safeUpdateItem->status() == UpdatesStatus::Default || m_safeUpdateItem->status() == UpdatesStatus::UpdateSucceeded){
        m_safeUpdateItem->setVisible(false);
    }

    if(m_storeUpdateItem->status() == UpdatesStatus::Default || m_storeUpdateItem->status() == UpdatesStatus::UpdateSucceeded){
        m_storeUpdateItem->setVisible(false);
    }

    if(m_unknownUpdateItem->status() == UpdatesStatus::Default || m_unknownUpdateItem->status() == UpdatesStatus::UpdateSucceeded){
        m_unknownUpdateItem->setVisible(false);
    }

}

void UpdateCtrlWidget::onChangeUpdatesAvailableStatus()
{

    //~ contents_path /update/Update
    setAllUpdateInfo(m_model->allDownloadInfo());

    showUpdateInfo();

    setLowBattery(m_model->lowBattery());
    setShowInfo(m_model->systemActivation());

    onRequestRefreshSize();
}

void UpdateCtrlWidget::onFullUpdateClicked()
{
    m_spinner->setVisible(true);
    m_spinner->start();
    m_updateingTipsLab->setVisible(true);
    m_fullUpdateBtn->setVisible(false);
    m_isUpdateingAll = true;

    auto sendRequestUpdates = [ = ](UpdateSettingItem * updateItem, ClassifyUpdateType type) {

        if(updateItem->status() == UpdatesStatus::DownloadPaused && updateItem->getCtrlButtonStatus()== ButtonStatus::start){
            int ctrlType = UpdateCtrlType::Start;
            Q_EMIT requestUpdateCtrl(type, ctrlType);
        }

        if(updateItem->status() != UpdatesStatus::Default
                || updateItem->status() == UpdatesStatus::UpdateFailed
                || updateItem->status() == UpdatesStatus::Downloaded
                || updateItem->status() == UpdatesStatus::Downloading
                || updateItem->status() == UpdatesStatus::UpdateFailed
                || updateItem->status() == UpdatesStatus::AutoDownloaded){
            Q_EMIT  requestUpdates(type);
        }

    };

    sendRequestUpdates(m_systemUpdateItem, ClassifyUpdateType::SystemUpdate);
    sendRequestUpdates(m_storeUpdateItem, ClassifyUpdateType::AppStoreUpdate);
    sendRequestUpdates(m_safeUpdateItem, ClassifyUpdateType::SecurityUpdate);
    sendRequestUpdates(m_unknownUpdateItem, ClassifyUpdateType::UnknownUpdate);
}

void UpdateCtrlWidget::onRequestUpdate(ClassifyUpdateType type)
{
    Q_EMIT requestUpdates(type);

    if (checkUpdateItemIsUpdateing(m_systemUpdateItem, type)
            && checkUpdateItemIsUpdateing(m_storeUpdateItem, type)
            && checkUpdateItemIsUpdateing(m_safeUpdateItem, type)
            && checkUpdateItemIsUpdateing(m_unknownUpdateItem, type)) {

        m_isUpdateingAll = true;
        m_spinner->setVisible(true);
        m_spinner->start();
        m_updateingTipsLab->setVisible(true);
        m_fullUpdateBtn->setVisible(false);
    }
}

void UpdateCtrlWidget::onRequestRefreshSize()
{
    m_updateSize = 0;
    auto refreshUpdateSize = [ = ](UpdateSettingItem * updateItem) {
        if (updateItem->status() == UpdatesStatus::UpdatesAvailable
                || updateItem->status() == UpdatesStatus::Downloading
                || updateItem->status() == UpdatesStatus::DownloadPaused
                || updateItem->status() == UpdatesStatus::UpdateFailed) {
            m_updateSize += updateItem->updateSize();
        }
    };

    refreshUpdateSize(m_systemUpdateItem);
    refreshUpdateSize(m_storeUpdateItem);
    refreshUpdateSize(m_safeUpdateItem);
    refreshUpdateSize(m_unknownUpdateItem);

    if (m_updateSize == 0) {
        m_CheckAgainBtn->setEnabled(false);
    }

    QString updateSize = formatCap(m_updateSize);
    updateSize = tr("Size") + ": " + updateSize;
    m_updateSizeLab->setText(updateSize);
}

bool UpdateCtrlWidget::checkUpdateItemIsUpdateing(UpdateSettingItem *updateItem, ClassifyUpdateType type)
{
    if (updateItem->classifyUpdateType() == type
            || updateItem->status() == UpdatesStatus::Default
            || updateItem->status() == UpdatesStatus::Downloading
            || updateItem->status() == UpdatesStatus::DownloadPaused
            || updateItem->status() == UpdatesStatus::Downloaded
            || updateItem->status() == UpdatesStatus::Installing
            || updateItem->status() == UpdatesStatus::UpdateSucceeded
            || updateItem->status() == UpdatesStatus::UpdateFailed
            || updateItem->status() == UpdatesStatus::RecoveryBackingup
            || updateItem->status() == UpdatesStatus::RecoveryBackingSuccessed
            || updateItem->status() == UpdatesStatus::RecoveryBackupFailed
            || updateItem->status() == UpdatesStatus::WaitRecoveryBackup) {
        return true;
    }
    return false;
}

void UpdateCtrlWidget::onRecoverBackupFinshed()
{
    auto sendRequestUpdates = [ = ](UpdateSettingItem * updateItem, ClassifyUpdateType type) {
        if (updateItem->isVisible() && updateItem->status() == UpdatesStatus::WaitRecoveryBackup) {
            Q_EMIT  requestUpdates(type);
        }
    };

    sendRequestUpdates(m_systemUpdateItem, ClassifyUpdateType::SystemUpdate);
    sendRequestUpdates(m_storeUpdateItem, ClassifyUpdateType::AppStoreUpdate);
    sendRequestUpdates(m_safeUpdateItem, ClassifyUpdateType::SecurityUpdate);
    sendRequestUpdates(m_unknownUpdateItem, ClassifyUpdateType::UnknownUpdate);

    m_CheckAgainBtn->setEnabled(false);
}

void UpdateCtrlWidget::onRecoverBackupFailed()
{
    if (m_systemUpdateItem->status() != UpdatesStatus::RecoveryBackupFailed && m_systemUpdateItem->status() == UpdatesStatus::WaitRecoveryBackup) {
        Q_EMIT requestUpdates(ClassifyUpdateType::SystemUpdate);
        return;
    }

    if (m_storeUpdateItem->status() != UpdatesStatus::RecoveryBackupFailed && m_storeUpdateItem->status() == UpdatesStatus::WaitRecoveryBackup) {
        Q_EMIT requestUpdates(ClassifyUpdateType::AppStoreUpdate);
        return;
    }

    if (m_safeUpdateItem->status() != UpdatesStatus::RecoveryBackupFailed && m_safeUpdateItem->status() == UpdatesStatus::WaitRecoveryBackup) {
        Q_EMIT requestUpdates(ClassifyUpdateType::SecurityUpdate);
        return;
    }

    if (m_unknownUpdateItem->status() != UpdatesStatus::RecoveryBackupFailed && m_unknownUpdateItem->status() == UpdatesStatus::WaitRecoveryBackup) {
        Q_EMIT requestUpdates(ClassifyUpdateType::UnknownUpdate);
        return;
    }

}

void UpdateCtrlWidget::onUpdateFailed()
{
    bool systemFailed = !m_systemUpdateItem->isVisible() || m_systemUpdateItem->status() == UpdatesStatus::UpdateFailed;
    bool appFailed = !m_storeUpdateItem->isVisible() || m_storeUpdateItem->status() == UpdatesStatus::UpdateFailed;
    bool safeFailed = !m_safeUpdateItem->isVisible() || m_safeUpdateItem->status() == UpdatesStatus::UpdateFailed;
    bool unknownFailed = !m_unknownUpdateItem->isVisible() || m_unknownUpdateItem->status() == UpdatesStatus::UpdateFailed;

    if (systemFailed && appFailed && safeFailed && unknownFailed) {
        m_CheckAgainBtn->setEnabled(true);
    }

}

void UpdateCtrlWidget::initUpdateItem(UpdateSettingItem *updateItem)
{
    if (updateItem->status() == UpdatesStatus::Default) {
        updateItem->setStatus(UpdatesStatus::UpdatesAvailable);
    }

    updateItem->setIconVisible(true);
}



