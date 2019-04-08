/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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
#include "updateitem.h"
#include "widgets/translucentframe.h"

#include <QVBoxLayout>

#include "updatemodel.h"
#include "loadingitem.h"
#include "widgets/labels/normallabel.h"

#define UpgradeWarningSize 500

namespace dcc{
namespace update{

UpdateCtrlWidget::UpdateCtrlWidget(UpdateModel *model, QWidget *parent)
    : ContentWidget(parent),
      m_model(nullptr),
      m_status(UpdatesStatus::Updated),
      m_checkGroup(new SettingsGroup),
      m_checkUpdateItem(new LoadingItem),
      m_resultGroup(new SettingsGroup),
      m_resultItem(new ResultItem),
      m_progress(new DownloadProgressBar),
      m_summaryGroup(new SettingsGroup),
      m_upgradeWarningGroup(new SettingsGroup),
      m_summary(new SummaryItem),
      m_upgradeWarning(new SummaryItem),
      m_powerTip(new TipsLabel),
      m_reminderTip(new TipsLabel(tr("Please restart to use the system and applications properly after updated"))),
      m_noNetworkTip(new TipsLabel(tr("Network disconnected, please retry after connected"))),
      m_qsettings(new QSettings(this))
{
    setTitle(tr("Update"));

    TranslucentFrame* widget = new TranslucentFrame();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(10);

    m_checkGroup->setVisible(false);
    m_checkGroup->appendItem(m_checkUpdateItem);

    m_resultGroup->setVisible(false);
    m_resultGroup->appendItem(m_resultItem);

    m_progress->setVisible(false);

    m_summaryGroup->setVisible(false);
    m_summaryGroup->appendItem(m_summary);

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

    layout->addSpacing(10);
    layout->addWidget(m_checkGroup);
    layout->addWidget(m_resultGroup);
    layout->addWidget(m_progress);
    layout->addWidget(m_upgradeWarningGroup);
    layout->addWidget(m_summaryGroup);
    layout->addWidget(m_powerTip);
    layout->addWidget(m_reminderTip);
    layout->addWidget(m_noNetworkTip);
    layout->addStretch();

    widget->setLayout(layout);
    setContent(widget);

    setModel(model);

    connect(m_progress, &DownloadProgressBar::clicked, this, &UpdateCtrlWidget::onProgressBarClicked);
}

UpdateCtrlWidget::~UpdateCtrlWidget()
{

}

void UpdateCtrlWidget::loadAppList(const QList<AppUpdateInfo>& infos)
{
    qDebug() << infos.count();
    QLayoutItem *item;
    while((item = m_summaryGroup->layout()->takeAt(1)) != NULL) {
        item->widget()->deleteLater();
        delete item;
    }

    for(const AppUpdateInfo& info : infos)
    {
        UpdateItem* item = new UpdateItem();
        item->setAppInfo(info);

        m_summaryGroup->appendItem(item);
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
        qWarning() << "unhandled status " << m_status;
        break;
    }
}

void UpdateCtrlWidget::setStatus(const UpdatesStatus &status)
{
    m_status = status;

    m_noNetworkTip->setVisible(false);
    m_resultGroup->setVisible(false);
    m_progress->setVisible(false);
    m_summaryGroup->setVisible(false);
    m_upgradeWarningGroup->setVisible(false);
    m_reminderTip->setVisible(false);
    m_checkGroup->setVisible(false);
    m_checkUpdateItem->setVisible(false);
    m_checkUpdateItem->setProgressBarVisible(false);

    switch (status) {
    case UpdatesStatus::Checking:
        m_checkGroup->setVisible(true);
        m_checkUpdateItem->setVisible(true);
        m_checkUpdateItem->setProgressBarVisible(true);
        m_checkUpdateItem->setMessage(tr("Checking for updates, please wait..."));
        break;
    case UpdatesStatus::UpdatesAvailable:
        m_progress->setVisible(true);
        m_summaryGroup->setVisible(true);
        m_progress->setMessage(tr("Download and install updates"));
        setDownloadInfo(m_model->downloadInfo());
        m_progress->setValue(100);
        setLowBattery(m_model->lowBattery());
        break;
    case UpdatesStatus::Downloading:
        m_progress->setVisible(true);
        m_summaryGroup->setVisible(true);
        m_progress->setValue(m_progress->minimum());
        m_progress->setMessage(tr("%1% downloaded (Click to pause)").arg(m_progress->value()));
        break;
    case UpdatesStatus::DownloadPaused:
        m_progress->setVisible(true);
        m_summaryGroup->setVisible(true);
        m_progress->setMessage(tr("%1% downloaded (Click to continue)").arg(m_progress->value()));
        break;
    case UpdatesStatus::Downloaded:
        m_progress->setVisible(true);
        m_summaryGroup->setVisible(true);
        m_progress->setValue(m_progress->maximum());
        m_progress->setMessage(tr("Install updates"));
        setDownloadInfo(m_model->downloadInfo());
        setLowBattery(m_model->lowBattery());
        break;
    case UpdatesStatus::Updated:
        m_checkGroup->setVisible(true);
        m_checkUpdateItem->setVisible(true);
        m_checkUpdateItem->setMessage(tr("Your system is up to date"));
        break;
    case UpdatesStatus::Installing:
        m_progress->setVisible(true);
        m_summaryGroup->setVisible(true);
        m_progress->setMessage(tr("Updating, please wait..."));
        break;
    case UpdatesStatus::UpdateSucceeded:
        m_resultItem->setSuccess(true);
        m_resultGroup->setVisible(true);
        m_reminderTip->setVisible(true);
        break;
    case UpdatesStatus::UpdateFailed:
        m_resultGroup->setVisible(true);
        m_resultItem->setSuccess(false);
        break;
    case UpdatesStatus::NeedRestart:
        m_checkGroup->setVisible(true);
        m_checkUpdateItem->setVisible(true);
        m_checkUpdateItem->setMessage(tr("The newest system installed, restart to take effect"));
        break;
    case UpdatesStatus::NoNetwork:
        m_resultGroup->setVisible(true);
        m_resultItem->setSuccess(false);
        m_noNetworkTip->setVisible(true);
        break;
    case UpdatesStatus::NoSpace:
        m_resultGroup->setVisible(true);
        m_resultItem->setSuccess(false);
        m_resultItem->setMessage(tr("Insufficient disk space, unable to update system."));
        break;
    case UpdatesStatus::DeependenciesBrokenError:
        m_resultGroup->setVisible(true);
        m_resultItem->setSuccess(false);
        m_resultItem->setMessage(tr("Dependency error, failed to detect the updates"));
        break;
    default:
        qWarning() << "unknown status!!!";
    }
}

void UpdateCtrlWidget::setDownloadInfo(DownloadInfo *downloadInfo)
{
    if (!downloadInfo)
        return;

    const QList<AppUpdateInfo> &apps = downloadInfo->appInfos();
    const qlonglong downloadSize = downloadInfo->downloadSize();

    int appCount = apps.length();
    for (const AppUpdateInfo &info : apps) {
        if (info.m_packageId == "dde") {
            appCount--;
        }
    }

    m_summary->setTitle(tr("%n application update(s) detected", "", appCount));

    for (const AppUpdateInfo &info : apps) {
        if (info.m_packageId == "dde") {
            if (!appCount) {
                m_summary->setTitle(tr("New system edition detected"));
            } else {
                m_summary->setTitle(tr("New system edition and %n application update(s) detected", "", appCount));
            }
            break;
        }
    }

    if (!downloadSize)
        m_summary->setDetails(tr("Downloaded"));
    else {
        m_summary->setDetails(QString(tr("Download size: %1").arg(formatCap(downloadSize))));

        if ((downloadSize / 1024) / 1024 >= m_qsettings->value("upgrade_waring_size", UpgradeWarningSize).toInt())
            m_upgradeWarningGroup->setVisible(true);
    }

    loadAppList(apps);
}

void UpdateCtrlWidget::setProgressValue(const double value)
{
    m_progress->setValue(value * 100);

    if (m_status == UpdatesStatus::Downloading) {
        m_progress->setMessage(tr("%1% downloaded (Click to pause)").arg(qFloor(value * 100)));
    }
}

void UpdateCtrlWidget::setLowBattery(const bool &lowBattery)
{
    if (m_status == UpdatesStatus::Downloaded || m_status == UpdatesStatus::UpdatesAvailable) {
        if(lowBattery) {
            m_powerTip->setText(tr("Your battery is lower than 50%, please plug in to continue"));
        } else {
            m_powerTip->setText(tr("Please ensure sufficient power to restart, and don't power off or unplug your machine"));
        }

        m_progress->setDisabled(lowBattery);
        m_powerTip->setVisible(lowBattery);
    }
}

void UpdateCtrlWidget::setUpdateProgress(const double value)
{
    m_checkUpdateItem->setProgressValue(value * 100);
}

void UpdateCtrlWidget::setModel(UpdateModel *model)
{
    m_model = model;

    connect(m_model, &UpdateModel::statusChanged, this, &UpdateCtrlWidget::setStatus);
    connect(m_model, &UpdateModel::lowBatteryChanged, this, &UpdateCtrlWidget::setLowBattery);
    connect(m_model, &UpdateModel::downloadInfoChanged, this, &UpdateCtrlWidget::setDownloadInfo);
    connect(m_model, &UpdateModel::upgradeProgressChanged, this, &UpdateCtrlWidget::setProgressValue);
    connect(m_model, &UpdateModel::updateProgressChanged, this, &UpdateCtrlWidget::setUpdateProgress);

    setUpdateProgress(m_model->updateProgress());
    setProgressValue(m_model->upgradeProgress());
    setStatus(m_model->status());
    setLowBattery(m_model->lowBattery());
    setDownloadInfo(m_model->downloadInfo());
}

}
}
