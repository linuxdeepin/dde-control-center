//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "updatectrlwidget.h"
#include "updatemodel.h"
#include "widgets/loadingitem.h"
#include "widgets/settingsgroup.h"
#include "widgets/summaryitem.h"
#include "widgets/downloadprogressbar.h"
#include "widgets/resultitem.h"
#include "widgets/loadingitem.h"
#include "widgets/updatesettingitem.h"
#include "widgets/systemupdateitem.h"
#include "widgets/safeupdateitem.h"
#include "widgets/unknownupdateitem.h"
#include "widgets/downloadprogressbar.h"
#include "widgets/updateiteminfo.h"
#include "widgets/resultitem.h"
#include "widgets/safeupdateitem.h"
#include "widgets/summaryitem.h"
#include "widgets/systemupdateitem.h"
#include "widgets/unknownupdateitem.h"
#include <DDialog>

#include <DFontSizeManager>
#include <DPalette>
#include <DSysInfo>
#include <DLabel>
#include <DSpinner>

#include <QVBoxLayout>
#include <QSettings>
#include <QPushButton>
#include <QScrollArea>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(DdcUpdateCtrlWidget, "dcc-update-ctrlwidget")

#define UpgradeWarningSize 500
#define FullUpdateBtnWidth 92

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

UpdateCtrlWidget::UpdateCtrlWidget(UpdateModel *model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_status(UpdatesStatus::Updated)
    , m_checkUpdateItem(new LoadingItem())
    , m_resultItem(new ResultItem())
    , m_progress(new DownloadProgressBar(parent))
    , m_fullProcess(new DownloadProgressBar(parent))
    , m_upgradeWarningGroup(new DCC_NAMESPACE::SettingsGroup)
    , m_summary(new SummaryItem)
    , m_upgradeWarning(new SummaryItem)
    , m_powerTip(new QLabel)
    , m_reminderTip(new QLabel)
    , m_noNetworkTip(new QLabel)
    , m_qsettings(new QSettings(this))
    , m_activeState(UiActiveState::Unknown)
    , m_updateList(new QWidget(parent))
    , m_authorizationPrompt(new QLabel(parent))
    , m_isUpdateingAll(false)
    , m_checkUpdateBtn(new QPushButton(parent))
    , m_lastCheckTimeTip(new QLabel(parent))
    , m_CheckAgainBtn(new QPushButton(tr("Check Again")))
    , m_lastCheckAgainTimeTip(new QLabel(parent))
    , m_versionTip(new DLabel(parent))
    , m_spinner(new DSpinner(parent))
    , m_updateTipsLab(new DLabel(parent))
    , m_updateSizeLab(new DLabel(parent))
    , m_updateingTipsLab(new DLabel(parent))
    , m_fullUpdateBtn(new QPushButton)
    , m_updateSize(0)
    , m_systemUpdateItem(new SystemUpdateItem(parent))
    , m_safeUpdateItem(new SafeUpdateItem(parent))
    , m_unknownUpdateItem(new UnknownUpdateItem(parent))
    , m_updateSummaryGroup(new SettingsGroup)
{
    setAccessibleName("UpdateCtrlWidget");
    m_checkUpdateItem->setAccessibleName("checkUpdateItem");
    m_checkUpdateItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_UpdateErrorInfoMap.insert(UpdateErrorType::NoError, { UpdateErrorType::NoError, "", "" });
    m_UpdateErrorInfoMap.insert(UpdateErrorType::NoSpace, { UpdateErrorType::NoSpace, tr("Update failed: insufficient disk space"), tr("") });
    m_UpdateErrorInfoMap.insert(UpdateErrorType::UnKnown, { UpdateErrorType::UnKnown, "", "" });
    m_UpdateErrorInfoMap.insert(UpdateErrorType::NoNetwork, { UpdateErrorType::NoNetwork, tr("Dependency error, failed to detect the updates"), tr("") });
    m_UpdateErrorInfoMap.insert(UpdateErrorType::DpkgInterrupted, { UpdateErrorType::DpkgInterrupted, "", "" });
    m_UpdateErrorInfoMap.insert(UpdateErrorType::DeependenciesBrokenError, { UpdateErrorType::DeependenciesBrokenError, "", "" });

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
    DFontSizeManager::instance()->bind(m_lastCheckTimeTip, DFontSizeManager::T8);
    m_lastCheckTimeTip->setForegroundRole(DPalette::BrightText);

    QHBoxLayout *updateTitleHLay = new QHBoxLayout;
    QVBoxLayout *updateTitleFirstVLay = new QVBoxLayout;

    m_updateTipsLab->setText(tr("Updates Available"));
    DFontSizeManager::instance()->bind(m_updateTipsLab, DFontSizeManager::T5, QFont::DemiBold);
    m_updateTipsLab->setForegroundRole(DPalette::TextTitle);
    m_updateTipsLab->setVisible(false);

    DFontSizeManager::instance()->bind(m_versionTip, DFontSizeManager::T8);
    m_versionTip->setForegroundRole(DPalette::BrightText);
    m_versionTip->setEnabled(false);
    connect(m_model, &UpdateModel::systemVersionChanged, this, &UpdateCtrlWidget::updateSystemVersionLabel, Qt::UniqueConnection);
    connect(m_model, &UpdateModel::systemActivationChanged, this, &UpdateCtrlWidget::updateSystemVersionLabel, Qt::UniqueConnection);
    updateSystemVersionLabel();

    updateTitleFirstVLay->addWidget(m_updateTipsLab);

    DFontSizeManager::instance()->bind(m_updateSizeLab, DFontSizeManager::T8);
    m_updateSizeLab->setForegroundRole(DPalette::TextTips);
    updateTitleFirstVLay->addWidget(m_updateSizeLab);

    updateTitleHLay->setContentsMargins(QMargins(22, 50, 20, 12));
    updateTitleHLay->addLayout(updateTitleFirstVLay);
    updateTitleHLay->addWidget(m_spinner, 1, Qt::AlignRight);
    m_spinner->setVisible(false);
    m_spinner->setFixedSize(16, 16);
    m_updateingTipsLab->setText(tr("Updating..."));
    m_updateingTipsLab->setVisible(false);
    DFontSizeManager::instance()->bind(m_updateingTipsLab, DFontSizeManager::T8);
    m_updateingTipsLab->setForegroundRole(DPalette::TextTips);
    updateTitleHLay->addSpacing(5);
    updateTitleHLay->addWidget(m_updateingTipsLab);
    QString text = tr("Download Updates");

    m_fullUpdateBtn->setText(text);
    m_fullUpdateBtn->setVisible(false);
    updateTitleHLay->addStretch();
    updateTitleHLay->addWidget(m_fullUpdateBtn);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addSpacing(10);
    layout->addWidget(m_versionTip, 0, Qt::AlignHCenter);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    layout->addWidget(m_upgradeWarningGroup);
    layout->addWidget(m_powerTip);
    layout->addWidget(m_summary);
    layout->addWidget(m_progress);
    layout->addLayout(fullProcesslayout);

    layout->addStretch();
    layout->addWidget(m_resultItem);
    layout->addWidget(m_checkUpdateItem);
    layout->addWidget(m_reminderTip);
    layout->addWidget(m_noNetworkTip);

    layout->addSpacing(20);
    layout->addWidget(m_checkUpdateBtn, 0, Qt::AlignCenter);
    layout->addSpacing(5);
    layout->addWidget(m_lastCheckTimeTip);
    layout->addLayout(updateTitleHLay);
    layout->addWidget(m_updateList, 1);
    layout->addStretch();
    setLayout(layout);

    QVBoxLayout *contentLayout = new QVBoxLayout;

    m_systemUpdateItem->setVisible(false);
    m_updateSummaryGroup->appendItem(m_systemUpdateItem);

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
    DFontSizeManager::instance()->bind(m_lastCheckAgainTimeTip, DFontSizeManager::T8);
    m_lastCheckAgainTimeTip->setForegroundRole(DPalette::BrightText);
    m_lastCheckAgainTimeTip->setEnabled(false);

    contentLayout->addSpacing(20);
    contentLayout->addWidget(m_CheckAgainBtn, 0, Qt::AlignCenter);
    contentLayout->addSpacing(5);
    contentLayout->addWidget(m_lastCheckAgainTimeTip);

    m_updateList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_updateList->setLayout(contentLayout);
    initModel();

    initConnect();
}


void UpdateCtrlWidget::initConnect()
{
    auto initUpdateItemConnect = [ = ](UpdateSettingItem * updateItem) {
        connect(updateItem, &UpdateSettingItem::requestUpdate, this, &UpdateCtrlWidget::requestUpdates);
        connect(updateItem, &UpdateSettingItem::requestUpdateCtrl, this, &UpdateCtrlWidget::requestUpdateCtrl);
        connect(updateItem, &UpdateSettingItem::requestRefreshSize, this, &UpdateCtrlWidget::onRequestRefreshSize);
        connect(updateItem, &UpdateSettingItem::requestRefreshWidget, this, &UpdateCtrlWidget::onRequestRefreshWidget);
        connect(updateItem, &UpdateSettingItem::requestFixError, this, &UpdateCtrlWidget::requestFixError);
    };

    initUpdateItemConnect(m_systemUpdateItem);
    initUpdateItemConnect(m_safeUpdateItem);
    initUpdateItemConnect(m_unknownUpdateItem);

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

    qCDebug(DdcUpdateCtrlWidget) << " ========= UpdateCtrlWidget::setStatus  " << status;
    if (m_model->systemActivation() == UiActiveState::Unauthorized || m_model->systemActivation() == UiActiveState::TrialExpired) {
        m_status = Inactive;
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
    m_versionTip->setVisible(false);
    m_updateTipsLab->setVisible(false);
    m_updateSizeLab->setVisible(false);
    m_fullUpdateBtn->setVisible(false);
    m_spinner->setVisible(false);
    m_updateingTipsLab->setVisible(false);

    auto showCheckButton = [this](const QString & caption) {
        emit m_model->updateCheckUpdateTime();
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
    case UpdatesStatus::Inactive:
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
    case UpdatesStatus::Updating:
        showUpdateInfo();
        onRequestRefreshSize();
        onRequestRefreshWidget();
        break;
    case UpdatesStatus::UpdateSucceeded:
        m_resultItem->setVisible(true);
        m_resultItem->setSuccess(ShowStatus::IsSuccessed);
        m_reminderTip->setVisible(true);
        break;
    case UpdatesStatus::UpdateFailed:
        setUpdateFailedInfo(updateJobErrorMessage());
        showCheckButton(tr("Check Again"));
        break;
    case UpdatesStatus::RecoveryBackupFailed:
    case UpdatesStatus::RecoveryBackupFailedDiskFull:
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
    default:
        qCDebug(DdcUpdateCtrlWidget) << "unknown status!!!";
        break;
    }
}

void UpdateCtrlWidget::setSystemUpdateStatus(const UpdatesStatus &status)
{
    m_systemUpdateItem->setStatus(status);
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
    if (m_status == UpdatesStatus::Updating || m_status == UpdatesStatus::UpdatesAvailable) {
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
        if (lowBattery)
            enable = !lowBattery;
        else
            enable = activation;

        m_fullUpdateBtn->setEnabled(enable);

        m_powerTip->setVisible(lowBattery);
    }
}

void UpdateCtrlWidget::setUpdateProgress(const double value)
{
    m_checkUpdateItem->setProgressValue(static_cast<int>(value * 100));
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


void UpdateCtrlWidget::initModel()
{
    qRegisterMetaType<UpdateErrorType>("UpdateErrorType");

    connect(m_model, &UpdateModel::statusChanged, this, &UpdateCtrlWidget::setStatus);

    connect(m_model, &UpdateModel::systemUpdateStatusChanged, m_systemUpdateItem, &UpdateSettingItem::onUpdateStatuChanged);
    connect(m_model, &UpdateModel::safeUpdateStatusChanged, m_safeUpdateItem, &UpdateSettingItem::onUpdateStatuChanged);
    connect(m_model, &UpdateModel::unkonowUpdateStatusChanged, m_unknownUpdateItem, &UpdateSettingItem::onUpdateStatuChanged);

    connect(m_model, &UpdateModel::lowBatteryChanged, this, &UpdateCtrlWidget::setLowBattery);

    connect(m_model, &UpdateModel::upgradeProgressChanged, this, &UpdateCtrlWidget::setProgressValue);
    connect(m_model, &UpdateModel::updateProgressChanged, this, &UpdateCtrlWidget::setUpdateProgress);

    connect(m_model, &UpdateModel::systemActivationChanged, this, &UpdateCtrlWidget::setActiveState);
    connect(m_model, &UpdateModel::classityUpdateJobErrorChanged, this, &UpdateCtrlWidget::onClassityUpdateJonErrorChanged);

    connect(m_model, &UpdateModel::systemUpdateInfoChanged, this, &UpdateCtrlWidget::setSystemUpdateInfo);
    connect(m_model, &UpdateModel::safeUpdateInfoChanged, this, &UpdateCtrlWidget::setSafeUpdateInfo);
    connect(m_model, &UpdateModel::unknownUpdateInfoChanged, this, &UpdateCtrlWidget::setUnkonowUpdateInfo);

    connect(m_model, &UpdateModel::systemUpdateProgressChanged, m_systemUpdateItem, &UpdateSettingItem::onUpdateProgressChanged);
    connect(m_model, &UpdateModel::safeUpdateProgressChanged, m_safeUpdateItem, &UpdateSettingItem::onUpdateProgressChanged);
    connect(m_model, &UpdateModel::unkonowUpdateProgressChanged, m_unknownUpdateItem, &UpdateSettingItem::onUpdateProgressChanged);

    connect(m_model, &UpdateModel::systemUpdateDownloadSizeChanged, m_systemUpdateItem, &UpdateSettingItem::setUpdateSize);
    connect(m_model, &UpdateModel::safeUpdateDownloadSizeChanged, m_safeUpdateItem, &UpdateSettingItem::setUpdateSize);
    connect(m_model, &UpdateModel::unkonowUpdateDownloadSizeChanged, m_unknownUpdateItem, &UpdateSettingItem::setUpdateSize);

    m_updateingItemMap.clear();

    setUpdateProgress(m_model->updateProgress());
    setProgressValue(m_model->upgradeProgress());

    setSystemUpdateStatus(m_model->getSystemUpdateStatus());
    setUnkonowUpdateStatus(m_model->getUnkonowUpdateStatus());
    setSystemUpdateInfo(m_model->systemDownloadInfo());
    setSafeUpdateInfo(m_model->safeDownloadInfo());
    setUnkonowUpdateInfo(m_model->unknownDownloadInfo());

    QMap<ClassifyUpdateType, UpdateErrorType> errorInfoMap = m_model->getUpdateErrorTypeMap();
    for (ClassifyUpdateType type : m_updateingItemMap.keys()) {
        if (errorInfoMap.contains(type)) {
            m_updateingItemMap.value(type)->setUpdateJobErrorMessage(errorInfoMap.value(type));
        }
    }

    if (errorInfoMap.contains(ClassifyUpdateType::Invalid) && errorInfoMap.contains(ClassifyUpdateType::Invalid)) {
        setUpdateJobErrorMessage(errorInfoMap.value(ClassifyUpdateType::Invalid));
    }

    qCDebug(DdcUpdateCtrlWidget) << "setModel" << m_model->status();
    qCDebug(DdcUpdateCtrlWidget) << "setModel" << "getSystemUpdateStatus" << m_model->getSystemUpdateStatus();
    qCDebug(DdcUpdateCtrlWidget) << "setModel" << "getSafeUpdateStatus" << m_model->getSafeUpdateStatus();
    qCDebug(DdcUpdateCtrlWidget) << "setModel" << "getUnkonowUpdateStatus" << m_model->getUnkonowUpdateStatus();

    if (m_model->enterCheckUpdate()) {
        setStatus(UpdatesStatus::Checking);
    } else {
        setStatus(m_model->status());
    }

    setLowBattery(m_model->lowBattery());
}

void UpdateCtrlWidget::updateSystemVersionLabel()
{
#ifndef DISABLE_ACTIVATOR
    if (m_model->systemActivation() == UiActiveState::Authorized || m_model->systemActivation() == UiActiveState::TrialAuthorized || m_model->systemActivation() == UiActiveState::AuthorizedLapse) {
        m_versionTip->setText(QString("%1: %2").arg(tr("Current Edition")).arg(m_model->systemVersionInfo()));
    } else {
        m_versionTip->clear();
    }
#else
    m_versionTip->setText(QString("%1: %2").arg(tr("Current Edition")).arg(m_model->systemVersionInfo()));
#endif
}

void UpdateCtrlWidget::setSystemUpdateInfo(UpdateItemInfo *updateItemInfo)
{
    m_updateingItemMap.remove(ClassifyUpdateType::SystemUpdate);
    if (nullptr == updateItemInfo) {
        m_systemUpdateItem->setVisible(false);
        return;
    }

    initUpdateItem(m_systemUpdateItem);
    m_systemUpdateItem->setData(updateItemInfo);
    m_updateingItemMap.insert(ClassifyUpdateType::SystemUpdate, m_systemUpdateItem);
}


void UpdateCtrlWidget::setSafeUpdateInfo(UpdateItemInfo *updateItemInfo)
{
    m_updateingItemMap.remove(ClassifyUpdateType::SecurityUpdate);
    if (nullptr == updateItemInfo) {
        m_safeUpdateItem->setVisible(false);
        return;
    }

    initUpdateItem(m_safeUpdateItem);
    m_safeUpdateItem->setData(updateItemInfo);
    m_updateingItemMap.insert(ClassifyUpdateType::SecurityUpdate, m_safeUpdateItem);
}

void UpdateCtrlWidget::setUnkonowUpdateInfo(UpdateItemInfo *updateItemInfo)
{
    m_updateingItemMap.remove(ClassifyUpdateType::UnknownUpdate);
    if (nullptr == updateItemInfo) {
        m_unknownUpdateItem->setVisible(false);
        return;
    }

    initUpdateItem(m_unknownUpdateItem);
    m_unknownUpdateItem->setData(updateItemInfo);
    m_updateingItemMap.insert(ClassifyUpdateType::UnknownUpdate, m_unknownUpdateItem);
}

void UpdateCtrlWidget::setAllUpdateInfo(QMap<ClassifyUpdateType, UpdateItemInfo *> updateInfoMap)
{
    m_updateingItemMap.clear();
    setSystemUpdateInfo(updateInfoMap.value(ClassifyUpdateType::SystemUpdate));
    setSafeUpdateInfo(updateInfoMap.value(ClassifyUpdateType::SecurityUpdate));
    setUnkonowUpdateInfo(updateInfoMap.value(ClassifyUpdateType::UnknownUpdate));
}

void UpdateCtrlWidget::showUpdateInfo()
{
    m_fullProcess->setVisible(false);
    m_updateList->setVisible(true);
    m_summary->setVisible(false);
    m_powerTip->setVisible(false);

    showAllUpdate();

    m_updateTipsLab->setVisible(true);
    m_updateSizeLab->setVisible(true);
    m_versionTip->setVisible(true);

    m_CheckAgainBtn->setVisible(true);
    emit m_model->updateCheckUpdateTime();
    m_lastCheckAgainTimeTip->setText(tr("Last checking time: ") + m_model->lastCheckUpdateTime());
    m_lastCheckAgainTimeTip->setVisible(true);
    m_updateSummaryGroup->setVisible(true);

    setSystemUpdateStatus(m_model->getSystemUpdateStatus());
    setSafeUpdateStatus(m_model->getSafeUpdateStatus());
    setUnkonowUpdateStatus(m_model->getUnkonowUpdateStatus());

    if (m_systemUpdateItem->status() == UpdatesStatus::Default || m_systemUpdateItem->status() == UpdatesStatus::UpdateSucceeded) {
        m_systemUpdateItem->setVisible(false);
    }

    if (m_safeUpdateItem->status() == UpdatesStatus::Default || m_safeUpdateItem->status() == UpdatesStatus::UpdateSucceeded) {
        m_safeUpdateItem->setVisible(false);
    }

    if (m_unknownUpdateItem->status() == UpdatesStatus::Default || m_unknownUpdateItem->status() == UpdatesStatus::UpdateSucceeded) {
        m_unknownUpdateItem->setVisible(false);
    }

}

void UpdateCtrlWidget::onChangeUpdatesAvailableStatus()
{
    showUpdateInfo();
    setAllUpdateInfo(m_model->allDownloadInfo());

    setLowBattery(m_model->lowBattery());
    setShowInfo(m_model->systemActivation());

    onRequestRefreshSize();
    onRequestRefreshWidget();
}

void UpdateCtrlWidget::onFullUpdateClicked()
{
    if (!m_model->getBackupUpdates()) {
        DDialog dialog;
        dialog.setFixedWidth(420);
        dialog.setTitle(tr("The backup update function is currently disabled. If the upgrade fails, the system cannot be rolled back!"));
        dialog.addButton(tr("Cancel"));
        dialog.addButton(tr("Update Now"), false, DDialog::ButtonWarning);
        int ret = dialog.exec();
        if (ret != DDialog::Accepted) {
           return;
        }
    }
    for (UpdateSettingItem *updateItem : m_updateingItemMap.values()) {
        if (updateItem->status() == UpdatesStatus::UpdatesAvailable
                || updateItem->status() == UpdatesStatus::UpdateFailed
                || updateItem->status() == UpdatesStatus::Downloaded
                || updateItem->status() == UpdatesStatus::Downloading
                || updateItem->status() == UpdatesStatus::DownloadPaused
                || updateItem->status() == UpdatesStatus::UpdateFailed
                || updateItem->status() == UpdatesStatus::AutoDownloaded) {
            Q_EMIT  requestUpdates(updateItem->classifyUpdateType());
        }
    }
}

void UpdateCtrlWidget::onRequestRefreshSize()
{
    m_updateSize = 0;

    for (UpdateSettingItem *updateItem : m_updateingItemMap.values()) {
        if (updateItem->status() != UpdatesStatus::Default
                && updateItem->status() != UpdatesStatus::Downloaded
                && updateItem->status() != UpdatesStatus::AutoDownloaded
                && updateItem->status() != UpdatesStatus::Installing
                && updateItem->status() != UpdatesStatus::Updated) {
            m_updateSize += updateItem->updateSize();
        }
    }

    if (m_updateSize == 0) {
        m_upgradeWarningGroup->setVisible(false);
    } else if ((static_cast<int64_t>(m_updateSize) / 1024) / 1024 >= m_qsettings->value("upgrade_waring_size", UpgradeWarningSize).toInt()) {
        m_upgradeWarningGroup->setVisible(true);
    } else {
        m_upgradeWarningGroup->setVisible(false);
    }

    QString updateSize = formatCap(m_updateSize);
    updateSize = tr("Size") + ": " + updateSize;
    m_updateSizeLab->setText(updateSize);
}

void UpdateCtrlWidget::onRequestRefreshWidget()
{
    m_isUpdateingAll = true;
    bool isUpdateing = false;
    QList<ClassifyUpdateType> removeItem;
    for (UpdateSettingItem *updateItem : m_updateingItemMap.values()) {
        if (updateItem->status() == UpdatesStatus::Default || updateItem->status() == UpdatesStatus::UpdateSucceeded) {
            removeItem.append(updateItem->classifyUpdateType());
            continue;
        }

        if (updateItem->status() == UpdatesStatus::AutoDownloaded
                || updateItem->status() == UpdatesStatus::UpdatesAvailable
                || updateItem->status() == UpdatesStatus::UpdateFailed
                || updateItem->status() == UpdatesStatus::RecoveryBackupFailed
                || updateItem->status() == UpdatesStatus::RecoveryBackupFailedDiskFull) {
            m_isUpdateingAll = false;
        } else {
            isUpdateing = true;
        }
    }

    for (ClassifyUpdateType type : removeItem) {
        m_updateingItemMap.remove(type);
    }

    if (isUpdateing) {
        m_CheckAgainBtn->setEnabled(false);
    } else {
        m_CheckAgainBtn->setEnabled(true);
    }

    showAllUpdate();
}

void UpdateCtrlWidget::showAllUpdate()
{
    m_spinner->setVisible(m_isUpdateingAll);
    if (m_isUpdateingAll) {
        m_spinner->start();
    } else {
        m_spinner->stop();
    }

    m_updateingTipsLab->setVisible(m_isUpdateingAll);
    m_fullUpdateBtn->setVisible(!m_isUpdateingAll);
}

UpdateErrorType UpdateCtrlWidget::updateJobErrorMessage() const
{
    return m_updateJobErrorMessage;
}

void UpdateCtrlWidget::setUpdateJobErrorMessage(const UpdateErrorType &updateJobErrorMessage)
{
    m_updateJobErrorMessage = updateJobErrorMessage;
}

void UpdateCtrlWidget::setUpdateFailedInfo(const UpdateErrorType &errorType)
{
    m_resultItem->setVisible(true);
    m_resultItem->setSuccess(ShowStatus::IsFailed);
    if (errorType == UpdateErrorType::NoNetwork) {
        m_noNetworkTip->setVisible(true);
        return;
    }
    m_resultItem->setMessage(m_UpdateErrorInfoMap.contains(errorType) ? m_UpdateErrorInfoMap.value(errorType).errorMessage : tr(""));
}

void UpdateCtrlWidget::initUpdateItem(UpdateSettingItem *updateItem)
{
    updateItem->setIconVisible(true);
}

void UpdateCtrlWidget::onClassityUpdateJonErrorChanged(const ClassifyUpdateType &type, const UpdateErrorType &errorType)
{
    switch (type) {
    case ClassifyUpdateType::Invalid:
        setUpdateJobErrorMessage(errorType);
        break;
    case ClassifyUpdateType::SystemUpdate:
        m_systemUpdateItem->setUpdateJobErrorMessage(errorType);
        break;
    case ClassifyUpdateType::SecurityUpdate:
        m_safeUpdateItem->setUpdateJobErrorMessage(errorType);
        break;
    case ClassifyUpdateType::UnknownUpdate:
        m_unknownUpdateItem->setUpdateJobErrorMessage(errorType);
        break;
    default:
        break;
    }
}

void UpdateCtrlWidget::onShowUpdateCtrl()
{
    if (m_model->getUpdatablePackages() && m_model->status() == UpdatesStatus::Default) {
        Q_EMIT m_model->beginCheckUpdate();
    }
}



