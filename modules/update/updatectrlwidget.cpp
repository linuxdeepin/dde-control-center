#include "updatectrlwidget.h"
#include "updateitem.h"
#include "translucentframe.h"

#include <QVBoxLayout>

#include "updatemodel.h"

namespace dcc{
namespace update{

UpdateCtrlWidget::UpdateCtrlWidget(UpdateModel *model, QWidget *parent)
    : ContentWidget(parent),
      m_model(nullptr),
      m_downloadInfo(nullptr),
      m_checkGroup(new SettingsGroup),
      m_checkUpdateItem(new CheckUpdateItem),
      m_progress(new DownloadProgressBar),
      m_summaryGroup(new SettingsGroup),
      m_summary(new SummaryItem),
      m_powerTip(new QLabel)
{
    setTitle(tr("Update"));

    TranslucentFrame* widget = new TranslucentFrame();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(3);

    m_checkGroup->setVisible(false);
    m_checkUpdateItem->setMessage(tr("Checking for updates, please wait..."));
    m_checkGroup->appendItem(m_checkUpdateItem);

    m_progress->setVisible(false);

    m_summaryGroup->setVisible(false);
    m_summaryGroup->appendItem(m_summary);

    m_powerTip->setWordWrap(true);
    m_powerTip->setVisible(false);

    layout->addWidget(m_checkGroup);
    layout->addWidget(m_progress);
    layout->addWidget(m_summaryGroup);
    layout->addWidget(m_powerTip);

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
        emit requestDownloadUpdates();
        break;
    case UpdatesStatus::Downloading:
        emit requestPauseDownload();
        break;
    case UpdatesStatus::DownloadPaused:
        emit requestResumeDownload();
        break;
    case UpdatesStatus::Downloaded:
        emit requestInstallUpdates();
        break;
    default:
        break;
    }
}

//void UpdateCtrlWidget::onStatus(bool useBattery, double percent)
//{
//    m_progress->setCurState(UpdateType::RebootInstall);
//    if(useBattery)
//    {
//        if(percent >= 50)
//        {
//            m_powerTip->setText(tr("Please ensure sufficient power to restart,"
//                                   " and don't power off or unplug your machine"));
//        }
//        else
//        {
//            m_powerTip->setText(tr("Your battery is lower than 50%, please plug in to continue"));
//        }
//        m_powerTip->show();
//    }
//    else
//    {
//        m_powerTip->setText(tr("Please ensure sufficient power to restart,"
//                               " and don't power off or unplug your machine"));
//    }
//}

void UpdateCtrlWidget::setStatus(const UpdatesStatus &status)
{
    if (m_status != status) {
        m_status = status;

        switch (status) {
        case UpdatesStatus::Checking:
            m_checkGroup->setVisible(true);
            m_progress->setVisible(false);
            m_summaryGroup->setVisible(false);
            break;
        case UpdatesStatus::UpdatesAvailable:
            m_checkGroup->setVisible(false);
            m_progress->setVisible(true);
            m_summaryGroup->setVisible(true);
            m_progress->setMessage(tr("Download Updates"));
            setDownloadInfo(m_model->downloadInfo());
            break;
        case UpdatesStatus::Downloading:
            m_checkGroup->setVisible(false);
            m_progress->setVisible(true);
            m_summaryGroup->setVisible(true);
            m_progress->setMessage(tr("%1 downloaded (Click to pause)").arg(m_progress->text()));
            break;
        case UpdatesStatus::DownloadPaused:
            m_checkGroup->setVisible(false);
            m_progress->setVisible(true);
            m_summaryGroup->setVisible(true);
            m_progress->setMessage(tr("%1 downloaded (Click to continue)").arg(m_progress->text()));
            break;
        case UpdatesStatus::Downloaded:
            m_checkGroup->setVisible(false);
            m_progress->setVisible(true);
            m_summaryGroup->setVisible(false);
            m_progress->setMessage(tr("Restart to install updates"));
            m_summary->setTitle(tr("Download completed"));
            break;
        default:
            qWarning() << "unknown status!!!";
        }
    }
}

void UpdateCtrlWidget::setDownloadInfo(DownloadInfo *downloadInfo)
{
    if (!downloadInfo) return;

    if (m_downloadInfo) {
        m_downloadInfo->deleteLater();
        m_downloadInfo = nullptr;
    }

    const QList<AppUpdateInfo> &apps = downloadInfo->appInfos();
    const qlonglong downloadSize = downloadInfo->downloadSize();

    m_progress->setMessage(tr("Download Updates"));
    m_summary->setTitle(tr("New system edition and %1 application updates detected").arg(apps.length()));
    m_summary->setDetails(QString(tr("Update size: %1").arg(formatCap(downloadSize, 1000))));

    loadAppList(apps);

    m_downloadInfo = downloadInfo;
    connect(m_downloadInfo, &DownloadInfo::downloadProgressChanged, m_progress, &DownloadProgressBar::setValue);
}

void UpdateCtrlWidget::setModel(UpdateModel *model)
{
    m_model = model;

    connect(m_model, &UpdateModel::statusChanged, this, &UpdateCtrlWidget::setStatus);

    setStatus(m_model->status());
}

}
}
