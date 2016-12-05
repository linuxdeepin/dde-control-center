#include "updatectrlwidget.h"
#include "updateitem.h"

#include <QVBoxLayout>

UpdateCtrlWidget::UpdateCtrlWidget(QWidget *parent)
    : ContentWidget(parent)
{
    QWidget* widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(3);

    m_group = new SettingsGroup();
    m_group->hide();

    m_checkGroup = new SettingsGroup();
    m_progress = new DownloadProgressBar();
    m_progress->hide();

    m_summary = new SummaryItem();
    m_group->appendItem(m_summary);

    m_checkUpdateItem = new CheckUpdateItem();
    m_checkUpdateItem->setTitle(tr("Checking for updates, please wait..."));
    m_checkGroup->appendItem(m_checkUpdateItem);

    m_powerTip = new QLabel();
    m_powerTip->setStyleSheet("background-color: red");
    m_powerTip->setWordWrap(true);
    m_powerTip->hide();

    layout->addWidget(m_checkGroup);
    layout->addWidget(m_progress);
    layout->addWidget(m_group);
    layout->addWidget(m_powerTip);

    widget->setLayout(layout);
    setContent(widget);

    connect(m_progress, &DownloadProgressBar::action, this, &UpdateCtrlWidget::actionType);
}

UpdateCtrlWidget::~UpdateCtrlWidget()
{
}

void UpdateCtrlWidget::setCurState(UpdateType type)
{
    m_progress->setCurState(type);
}

void UpdateCtrlWidget::loadAppList(const QList<AppUpdateInfo>& infos)
{
    qDebug()<<Q_FUNC_INFO<<infos.count();

    QLayoutItem *item;
    while((item = m_group->layout()->takeAt(1)) != NULL) {
        item->widget()->deleteLater();
        delete item;
    }

    for(const AppUpdateInfo& info : infos)
    {
        UpdateItem* item = new UpdateItem();
        item->setAppInfo(info);

        m_group->appendItem(item);
    }
}

void UpdateCtrlWidget::updateDownloadProgress(double progress)
{
    int value = int(progress*100) ;
    m_progress->setValue(value);
    m_summary->setDetails(QString(tr("Downloaded %1/%2").arg(formatCap(progress*m_total, 1000)).arg(formatCap(m_total, 1000))));

    if(value == 100)
    {
        m_group->hide();
        m_powerTip->show();
    }
}

void UpdateCtrlWidget::onStatus(bool useBattery, double percent)
{
    m_progress->setCurState(UpdateType::RebootInstall);
    if(useBattery)
    {
        if(percent >= 50)
        {
            m_powerTip->setText(tr("Please ensure sufficient power to restart,"
                                   " and don't power off or unplug your machine"));
        }
        else
        {
            m_powerTip->setText(tr("Your battery is lower than 50%, please plug in to continue"));
        }
        m_powerTip->show();
    }
    else
    {
        m_powerTip->setText(tr("Please ensure sufficient power to restart,"
                               " and don't power off or unplug your machine"));
    }
}

void UpdateCtrlWidget::onPackagesDownloadSize(qlonglong size, int count)
{
    if (size != -1) {
        m_checkGroup->hide();
        // packages are already downloaded.
        if (size == 0)  {
            // Download completed
            m_progress->setCurState(UpdateType::RebootInstall);
            m_summary->setTitle(tr("Download completed"));
        } else {
            // display total download size
            m_progress->setCurState(UpdateType::CheckUpdate);
            m_summary->setTitle(tr("New system edition and %1 application updates detected").arg(count));
            m_total = size;
            m_summary->setDetails(QString(tr("Update size: %1").arg(formatCap(size, 1000))));
            m_group->show();
        }
        m_progress->show();
    }
}
