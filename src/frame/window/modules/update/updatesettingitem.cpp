//
// Created by swq on 2021/9/7.
//

#include "updatesettingitem.h"
#include "widgets/basiclistdelegate.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <qpushbutton.h>
#include <QIcon>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::update;

UpdateSettingItem::UpdateSettingItem(QWidget *parent)
    : SettingsItem(parent)
    , m_icon(new SmallLabel(this))
    , m_status(UpdatesStatus::Default)
    , m_updateSize(0)
    , m_progressVlaue(0)
    , m_updateJobErrorMessage("")
    , m_controlWidget(new updateControlPanel(this))
    , m_settingsGroup(new dcc::widgets::SettingsGroup(this, SettingsGroup::BackgroundStyle::NoneBackground))
{
    initUi();
    initConnect();
}

void UpdateSettingItem::initUi()
{
    m_icon->setFixedSize(48, 48);
    m_icon->setVisible(false);
    QWidget *widget = new QWidget();
    QVBoxLayout *vboxLay = new QVBoxLayout(widget);
    vboxLay->addWidget(m_icon);
    vboxLay->setContentsMargins(10, 6, 10, 10);
    widget->setLayout(vboxLay);

    QHBoxLayout *main = new QHBoxLayout;
    m_settingsGroup->appendItem(m_controlWidget);
    main->addWidget(widget, 0, Qt::AlignTop);
    main->addWidget(m_settingsGroup, 0, Qt::AlignTop);
    setLayout(main);

}

void UpdateSettingItem::setIconVisible(bool show)
{
    m_icon->setVisible(show);
}

void UpdateSettingItem::setIcon(QString path)
{
    const qreal ratio = devicePixelRatioF();
    QPixmap pix = loadPixmap(path).scaled(m_icon->size() * ratio,
                                          Qt::KeepAspectRatioByExpanding,
                                          Qt::SmoothTransformation);
    m_icon->setPixmap(pix);
}

void UpdateSettingItem::showMore()
{
    return;
}

ClassifyUpdateType UpdateSettingItem::classifyUpdateType() const
{
    return m_classifyUpdateType;
}

void UpdateSettingItem::setClassifyUpdateType(const ClassifyUpdateType &classifyUpdateType)
{
    m_classifyUpdateType = classifyUpdateType;
}

UpdatesStatus UpdateSettingItem::status() const
{
    return m_status;
}

void UpdateSettingItem::setStatus(const UpdatesStatus &status)
{
    qDebug() << "UpdateSettingItem::setStatus: " << status;

    if (status == UpdatesStatus::RecoveryBackingSuccessed && m_status != UpdatesStatus::RecoveryBackingup && m_status != UpdatesStatus::WaitRecoveryBackup) {
        return;
    }

    if (status == UpdatesStatus::RecoveryBackupFailed && m_status != UpdatesStatus::RecoveryBackingup) {
        return;
    }

    m_status = status;
    Q_EMIT requestRefreshWidget();
    this->setVisible(true);

    switch (m_status) {
    case UpdatesStatus::Default:
        this->setVisible(false);
        break;
    case UpdatesStatus::UpdatesAvailable:
        m_controlWidget->showUpdateProcess(false);
        m_controlWidget->setUpdateButtonVisible(true);
        setVisible(true);
        break;
    case UpdatesStatus::Downloading:
        m_controlWidget->showUpdateProcess(true);
        m_controlWidget->setProgressType(UpdateDProgressType::Download);
        setProgress(m_progressVlaue);
        m_controlWidget-> setButtonStatus(ButtonStatus::pause);
        m_controlWidget->setCtrlButtonEnabled(true);
        break;
    case UpdatesStatus::DownloadPaused:
        m_controlWidget->showUpdateProcess(true);
        m_controlWidget->setProgressType(UpdateDProgressType::Paused);
        m_controlWidget->setButtonStatus(ButtonStatus::start);
        break;
    case UpdatesStatus::Downloaded:
        m_controlWidget->showUpdateProcess(true);
        m_controlWidget->setProgressType(UpdateDProgressType::Download);
        setProgressVlaue(1.0);
        m_controlWidget->setButtonIcon(ButtonStatus::invalid);
        m_controlWidget->setCtrlButtonEnabled(false);
        Q_EMIT requestRefreshSize();
        break;
    case UpdatesStatus::AutoDownloaded:
        m_controlWidget->showUpdateProcess(false);
        Q_EMIT requestRefreshSize();
        break;
    case UpdatesStatus::Installing:
        m_controlWidget->showUpdateProcess(true);
        m_controlWidget->setProgressType(UpdateDProgressType::Install);
        setProgressVlaue(0.0);
        m_controlWidget->setButtonIcon(ButtonStatus::invalid);
        m_controlWidget->setCtrlButtonEnabled(false);
        break;
    case UpdatesStatus::UpdateSucceeded:
        m_controlWidget->setProgressType(UpdateDProgressType::Install);
        m_controlWidget->setButtonIcon(ButtonStatus::invalid);
        this->setVisible(false);
        break;
    case UpdatesStatus::UpdateFailed:
        m_controlWidget->showUpdateProcess(true);
        m_controlWidget->setProgressText(m_updateJobErrorMessage);
        m_controlWidget->showButton(true);
        m_controlWidget->setCtrlButtonEnabled(true);
        m_controlWidget->setButtonStatus(ButtonStatus::retry);
        break;
    case UpdatesStatus::NeedRestart:
        m_controlWidget->setProgressText(tr("The newest system installed, restart to take effect"));
        m_controlWidget->showButton(false);
        break;
    case UpdatesStatus::WaitRecoveryBackup:
        m_controlWidget->showUpdateProcess(true);
        m_controlWidget->setProgressText(tr("Waiting"));
        m_controlWidget->showButton(false);
        break;
    case UpdatesStatus::RecoveryBackingup:
        m_controlWidget->showUpdateProcess(true);
        m_controlWidget->setButtonIcon(ButtonStatus::invalid);
        m_controlWidget->showButton(false);
        m_controlWidget->setProgressType(UpdateDProgressType::Backup);
        m_controlWidget->setProgressText(tr("Backing up"));
        break;
    case UpdatesStatus::RecoveryBackingSuccessed:
        m_controlWidget->showUpdateProcess(true);
        setProgressVlaue(1.0);
        m_controlWidget->setProgressType(UpdateDProgressType::Backup);
        m_controlWidget->setButtonIcon(ButtonStatus::invalid);
        m_controlWidget->setCtrlButtonEnabled(false);
        break;
    case UpdatesStatus::RecoveryBackupFailed:
        m_controlWidget->showUpdateProcess(true);
        m_controlWidget->setProgressType(UpdateDProgressType::Backup);
        m_controlWidget->setProgressText(tr("System backup failed"));
        m_controlWidget->showButton(true);
        m_controlWidget->setCtrlButtonEnabled(true);
        m_controlWidget->setButtonStatus(ButtonStatus::retry);
        break;
    default:
        qDebug() << "unknown status!!!";
        break;
    }
}

void UpdateSettingItem::setProgress(double value)
{
    m_controlWidget->setProgressValue(static_cast<int>(value * 100));
}

ButtonStatus UpdateSettingItem::getCtrlButtonStatus()
{
    return m_controlWidget->getButtonStatus();
}

void UpdateSettingItem::setData(UpdateItemInfo *updateItemInfo)
{
    if (updateItemInfo == nullptr) {
        return;
    }

    m_controlWidget->setDate(tr("Release date: ") + updateItemInfo->updateTime());
    m_controlWidget->setVersion(tr("Version") + ": " + updateItemInfo->availableVersion());
    m_controlWidget->setTitle(updateItemInfo->name());
    m_controlWidget->setDetail(updateItemInfo->explain());

    setProgressVlaue(updateItemInfo->downloadProgress());
    setUpdateSize(updateItemInfo->downloadSize());
}

void UpdateSettingItem::onUpdateStatuChanged(const UpdatesStatus &status)
{
    if (m_status != status) {
        setStatus(status);
    }
}

void UpdateSettingItem::onUpdateProgressChanged(const double &value)
{
    setProgressVlaue(value);
}

QString UpdateSettingItem::getUpdateJobErrorMessage() const
{
    return m_updateJobErrorMessage;
}

void UpdateSettingItem::setUpdateJobErrorMessage(const QString &updateJobErrorMessage)
{
    m_updateJobErrorMessage = updateJobErrorMessage;
}

double UpdateSettingItem::getProgressVlaue() const
{
    return m_progressVlaue;
}

void UpdateSettingItem::setProgressVlaue(double progressVlaue)
{
    m_progressVlaue = progressVlaue;
    setProgress(progressVlaue);
}

qlonglong UpdateSettingItem::updateSize() const
{
    return m_updateSize;
}

void UpdateSettingItem::setUpdateSize(const qlonglong &updateSize)
{
    m_updateSize = updateSize;
}

void UpdateSettingItem::setLowBattery(bool lowBattery)
{
    m_controlWidget->setUpdateButtonEnable(lowBattery);
}

void UpdateSettingItem::initConnect()
{
    connect(m_controlWidget, &updateControlPanel::showDetail, this, &UpdateSettingItem::showMore);
    connect(m_controlWidget, &updateControlPanel::startUpdate, this, &UpdateSettingItem::onStartUpdate);
    connect(m_controlWidget, &updateControlPanel::StartDownload, this, &UpdateSettingItem::onStartDownload);
    connect(m_controlWidget, &updateControlPanel::PauseDownload, this, &UpdateSettingItem::onPauseDownload);
    connect(m_controlWidget, &updateControlPanel::RetryUpdate, this, &UpdateSettingItem::onRetryUpdate);
}

void UpdateSettingItem::onStartUpdate()
{
    Q_EMIT requestUpdate(m_classifyUpdateType);
}

void UpdateSettingItem::onStartDownload()
{
    int ctrlType = UpdateCtrlType::Start;
    Q_EMIT requestUpdateCtrl(m_classifyUpdateType, ctrlType);
}

void UpdateSettingItem::onPauseDownload()
{
    int ctrlType = UpdateCtrlType::Pause;
    Q_EMIT requestUpdateCtrl(m_classifyUpdateType, ctrlType);
}

void UpdateSettingItem::onRetryUpdate()
{
    m_controlWidget->setProgressType(UpdateDProgressType::InvalidType);
    setProgressVlaue(0);
    m_controlWidget->showButton(false);
    onStartUpdate();
}
