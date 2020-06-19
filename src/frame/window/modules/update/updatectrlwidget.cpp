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
    , m_checkUpdateItem(new LoadingItem)
    , m_resultItem(new ResultItem)
    , m_progress(new DownloadProgressBar)
    , m_fullProcess(new DownloadProgressBar)
    , m_summaryGroup(new SettingsGroup)
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
    , m_updateList(new ContentWidget)
    , m_authorizationPrompt(new TipsLabel)
    , m_checkUpdateBtn(new QPushButton)
    , m_lastCheckTimeTip(new TipsLabel)
{
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

    m_summaryGroup->setVisible(true);

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

    m_checkUpdateBtn->setFixedSize(QSize(300,36));
    m_checkUpdateBtn->setVisible(false);
    m_lastCheckTimeTip->setAlignment(Qt::AlignCenter);
    m_lastCheckTimeTip->setVisible(false);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter); 
    layout->addWidget(m_upgradeWarningGroup);
    layout->addWidget(m_powerTip);  
    layout->addWidget(m_summary);
    layout->addWidget(m_progress);
    layout->addLayout(fullProcesslayout);
    layout->addWidget(m_updateList ,1);

    layout->addStretch();
    layout->addWidget(m_resultItem);
    layout->addWidget(m_checkUpdateItem);
    layout->addWidget(m_reminderTip);
    layout->addWidget(m_noNetworkTip);
    layout->addSpacing(20);
    layout->addWidget(m_checkUpdateBtn);
    layout->addSpacing(5);
    layout->addWidget(m_lastCheckTimeTip);
    layout->addStretch();
    setLayout(layout);

    QWidget *contentWidget = new QWidget;
    QVBoxLayout *contentLayout = new QVBoxLayout;
    contentLayout->addWidget(m_summaryGroup);
    contentLayout->addStretch();
    contentWidget->setLayout(contentLayout);
    m_updateList->setMinimumHeight(height() - 150);
    m_updateList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_updateList->setContent(contentWidget);

    setModel(model);

    connect(m_progress, &DownloadProgressBar::clicked, this, &UpdateCtrlWidget::onProgressBarClicked);
    connect(m_fullProcess, &DownloadProgressBar::clicked, this, &UpdateCtrlWidget::onProgressBarClicked);
    connect(m_checkUpdateBtn, &QPushButton::clicked, m_model, &UpdateModel::beginCheckUpdate);
    connect(m_checkUpdateBtn, &QPushButton::clicked, [this] {
        this->setFocus();
    });
}

UpdateCtrlWidget::~UpdateCtrlWidget()
{

}

void UpdateCtrlWidget::setShowInfo(const bool value)
{
    m_fullProcess->setEnabled(value);
    m_authorizationPrompt->setVisible(UpdatesStatus::UpdatesAvailable == m_model->status() && !value);
}

void UpdateCtrlWidget::loadAppList(const QList<AppUpdateInfo> &infos)
{
//    qDebug() << infos.count();

    QLayoutItem *item;
    while ((item = m_summaryGroup->layout()->takeAt(0)) != nullptr) {
        item->widget()->deleteLater();
        delete item;
    }

    for (const AppUpdateInfo &info : infos) {
        UpdateItem *item = new UpdateItem();
        item->setAppInfo(info);

        m_summaryGroup->appendItem(item);
    }
    //在只有一个更新的时候,为防止item过度的拉伸
    if (infos.count() > 1) {
        m_summaryGroup->getLayout()->addStretch();
    } else {
        m_summaryGroup->getLayout()->addStretch(1);
    }
}

void UpdateCtrlWidget::onProgressBarClicked()
{
    switch (m_status) {
    case UpdatesStatus::UpdatesAvailable:
        Q_EMIT requestDownloadUpdates();
        break;
    case UpdatesStatus::Downloading:
        Q_EMIT requestPauseDownload();
        break;
    case UpdatesStatus::DownloadPaused:
        Q_EMIT requestResumeDownload();
        break;
    case UpdatesStatus::Downloaded:
        Q_EMIT requestInstallUpdates();
        break;
    default:
        qDebug() << "unhandled status " << m_status;
        break;
    }
}

void UpdateCtrlWidget::setStatus(const UpdatesStatus &status)
{
    m_status = status;

    if (!m_model->systemActivation()) {
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
    m_checkUpdateItem->setVisible(false);
    m_checkUpdateItem->setProgressBarVisible(false);
    m_checkUpdateItem->setImageAndTextVisible(false);
    m_summary->setVisible(false);
    m_checkUpdateBtn->setVisible(false);
    m_lastCheckTimeTip->setVisible(false);

    auto showCheckButton = [this](const QString & caption) {
        m_model->updateCheckUpdateTime();
        m_checkUpdateBtn->setText(caption);
        m_checkUpdateBtn->setVisible(true);
        m_lastCheckTimeTip->setText(tr("Last checking time: ") + m_model->lastCheckUpdateTime());
        m_lastCheckTimeTip->setVisible(true);
    };

    switch (m_status) {
    case UpdatesStatus::Default:
        showCheckButton(tr("Check for Updates"));
        break;
    case UpdatesStatus::NoAtive:
        m_resultItem->setVisible(true);
        m_resultItem->setSuccess(ShowStatus::NoActive);
        break;
    case UpdatesStatus::Checking:
        m_model->beginCheckUpdate();
        m_checkUpdateItem->setVisible(true);
        m_checkUpdateItem->setVisible(true);
        m_checkUpdateItem->setProgressBarVisible(true);
        m_checkUpdateItem->setMessage(tr("Checking for updates, please wait..."));
        m_checkUpdateItem->setImageOrTextVisible(false);
        break;
    case UpdatesStatus::UpdatesAvailable:
        m_fullProcess->setVisible(true);
        m_updateList->setVisible(true);
        m_summary->setVisible(true);
        //~ contents_path /update/Update
        m_fullProcess->setMessage(tr("Download and install updates"));
        setDownloadInfo(m_model->downloadInfo());
        setLowBattery(m_model->lowBattery());
        setShowInfo(m_model->systemActivation());
        m_checkUpdateBtn->setText(tr("Check Again"));
        m_checkUpdateBtn->setVisible(true);
        m_lastCheckTimeTip->setText(tr("Last checking time: ") + m_model->lastCheckUpdateTime());
        m_lastCheckTimeTip->setVisible(true);
        break;
    case UpdatesStatus::Downloading:
        m_progress->setVisible(true);
        m_updateList->setVisible(true);
        m_summary->setVisible(true);
        m_progress->setMessage(tr("%1% downloaded (Click to pause)").arg(m_progress->value()));
        m_progress->setProcessValue(m_progress->value());
        break;
    case UpdatesStatus::DownloadPaused:
        m_progress->setVisible(true);
        m_updateList->setVisible(true);
        m_summary->setVisible(true);
        m_progress->setMessage(tr("%1% downloaded (Click to continue)").arg(m_progress->value()));
        break;
    case UpdatesStatus::Downloaded:
        m_fullProcess->setVisible(true);
        m_updateList->setVisible(true);
        m_summary->setVisible(true);
        //~ contents_path /update/Update
        m_fullProcess->setMessage(tr("Install updates"));
        setDownloadInfo(m_model->downloadInfo());
        setLowBattery(m_model->lowBattery());
        break;
    case UpdatesStatus::Updated:
        m_checkUpdateItem->setVisible(true);
        m_checkUpdateItem->setVisible(true);
        m_checkUpdateItem->setMessage(tr("Your system is up to date"));
        m_checkUpdateItem->setImageOrTextVisible(true);
        m_checkUpdateItem->setSystemVersion(m_systemVersion);
        showCheckButton(tr("Check for Updates"));
        break;
    case UpdatesStatus::Installing:
        m_progress->setVisible(true);
        m_updateList->setVisible(true);
        m_summary->setVisible(true);
        m_progress->setMessage(tr("Updating, please wait..."));
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
    case UpdatesStatus::RecoveryBackingup:
    case UpdatesStatus::RecoveryBackingSuccessed:
        m_progress->setVisible(true);
        m_updateList->setVisible(true);
        m_summary->setVisible(true);
        m_progress->setMessage(tr("Backing up, please wait..."));
        break;
    case UpdatesStatus::RecoveryBackupFailed:
        m_resultItem->setVisible(true);
        m_resultItem->setSuccess(ShowStatus::IsFailed);
        m_resultItem->setMessage(tr("System backup failed"));
        break;
    default:
        qDebug() << "unknown status!!!";
        break;
    }
}

void UpdateCtrlWidget::setDownloadInfo(DownloadInfo *downloadInfo)
{
    if (!downloadInfo)
        return;

    const QList<AppUpdateInfo> &apps = downloadInfo->appInfos();
    const qlonglong downloadSize = downloadInfo->downloadSize();  //+ 6-16-2

    int appCount = apps.length();
    for (const AppUpdateInfo &info : apps) {
        if (info.m_packageId == "dde") {
            appCount--;
        }
    }

    m_summary->setTitle(tr("%n application update(s) available", "", appCount));

    for (const AppUpdateInfo &info : apps) {
        if (info.m_packageId == "dde") {
            if (!appCount) {
                m_summary->setTitle(tr("New system edition available"));
            } else {
                m_summary->setTitle(QString("New system edition %1 and %2 application update(s) available").arg(m_systemVersion).arg(appCount));
            }
            break;
        }
    }

    if (!downloadSize) {
        m_summary->setDetails(tr("Downloaded"));
    } else {
        m_summary->setDetails(QString(tr("Size: %1").arg(formatCap(downloadSize))));

        if ((downloadSize / 1024) / 1024 >= m_qsettings->value("upgrade_waring_size", UpgradeWarningSize).toInt())
            m_upgradeWarningGroup->setVisible(true);
    }

    loadAppList(apps);
}

void UpdateCtrlWidget::setProgressValue(const double value)
{
    m_progress->setProcessValue(value * 100);

    if (m_status == UpdatesStatus::Downloading) {
        m_progress->setMessage(tr("%1% downloaded (Click to pause)").arg(qFloor(value * 100)));
    } else if (m_status == UpdatesStatus::DownloadPaused) {
        m_progress->setMessage(tr("%1% downloaded (Click to continue)").arg(qFloor(value * 100)));
    }
}

void UpdateCtrlWidget::setLowBattery(const bool &lowBattery)
{
    if (m_status == UpdatesStatus::Downloaded || m_status == UpdatesStatus::UpdatesAvailable) {
        if (lowBattery) {
            m_powerTip->setText(tr("Your battery is lower than 50%, please plug in to continue"));
        } else {
            m_powerTip->setText(tr("Please ensure sufficient power to restart, and don't power off or unplug your machine"));
        }

        m_fullProcess->setDisabled(lowBattery);
        m_powerTip->setVisible(lowBattery);
    }
}

void UpdateCtrlWidget::setUpdateProgress(const double value)
{
    m_checkUpdateItem->setProgressValue(value * 100);
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

void UpdateCtrlWidget::setModel(UpdateModel *model)
{
    m_model = model;

    connect(m_model, &UpdateModel::statusChanged, this, &UpdateCtrlWidget::setStatus);
    connect(m_model, &UpdateModel::lowBatteryChanged, this, &UpdateCtrlWidget::setLowBattery);
    connect(m_model, &UpdateModel::downloadInfoChanged, this, &UpdateCtrlWidget::setDownloadInfo);
    connect(m_model, &UpdateModel::upgradeProgressChanged, this, &UpdateCtrlWidget::setProgressValue);
    connect(m_model, &UpdateModel::updateProgressChanged, this, &UpdateCtrlWidget::setUpdateProgress);
    connect(m_model, &UpdateModel::recoverBackingUpChanged, this, &UpdateCtrlWidget::setRecoverBackingUp);
    connect(m_model, &UpdateModel::recoverConfigValidChanged, this, &UpdateCtrlWidget::setRecoverConfigValid);
    connect(m_model, &UpdateModel::recoverRestoringChanged, this, &UpdateCtrlWidget::setRecoverRestoring);

    setUpdateProgress(m_model->updateProgress());
    setProgressValue(m_model->upgradeProgress());

    if (m_model->enterCheckUpdate()) {
        setStatus(UpdatesStatus::Checking);
    } else {
        setStatus(m_model->status());
    }

    setLowBattery(m_model->lowBattery());
    setDownloadInfo(m_model->downloadInfo());

    setDownloadInfo(m_model->downloadInfo());
}

void UpdateCtrlWidget::setSystemVersion(QString version)
{
    if (m_systemVersion != version) {
        m_systemVersion = version;
    }
}
