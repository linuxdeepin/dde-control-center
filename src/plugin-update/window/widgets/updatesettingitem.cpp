//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "updatesettingitem.h"

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <qpushbutton.h>
#include <QIcon>

#include <DFloatingButton>
#include <DCommandLinkButton>
#include <DLabel>
#include <DLineEdit>
#include <DTextEdit>
#include <DTipLabel>
#include <DShadowLine>

UpdateSettingItem::UpdateSettingItem(QWidget *parent)
    : SettingsItem(parent)
    , m_icon(new QLabel(this))
    , m_status(UpdatesStatus::Default)
    , m_updateSize(0)
    , m_progressVlaue(0)
    , m_updateJobErrorMessage(UpdateErrorType::NoError)
    , m_controlWidget(new updateControlPanel(this))
    , m_settingsGroup(new DCC_NAMESPACE::SettingsGroup(this, DCC_NAMESPACE::SettingsGroup::BackgroundStyle::NoneBackground))
{
    m_UpdateErrorInfoMap.insert(UpdateErrorType::NoError, { UpdateErrorType::NoError, "", "" });
    m_UpdateErrorInfoMap.insert(UpdateErrorType::NoSpace, { UpdateErrorType::NoSpace, tr("Insufficient disk space"), tr("Update failed: insufficient disk space") });
    m_UpdateErrorInfoMap.insert(UpdateErrorType::UnKnown, { UpdateErrorType::UnKnown, tr("Update failed"), "" });
    m_UpdateErrorInfoMap.insert(UpdateErrorType::NoNetwork, { UpdateErrorType::NoNetwork, tr("Network error"), tr("Network error, please check and try again") });
    m_UpdateErrorInfoMap.insert(UpdateErrorType::DpkgInterrupted, { UpdateErrorType::DpkgInterrupted, tr("Packages error"), tr("Packages error, please try again") });
    m_UpdateErrorInfoMap.insert(UpdateErrorType::DeependenciesBrokenError, { UpdateErrorType::DeependenciesBrokenError, tr("Dependency error"), tr("Unmet dependencies") });

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
    main->setMargin(0);
    main->setSpacing(0);
    main->setContentsMargins(10, 10, 0, 0);
    m_settingsGroup->appendItem(m_controlWidget);
    m_settingsGroup->setSpacing(0);
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

    m_status = status;
    this->setVisible(true);

    switch (m_status) {
    case UpdatesStatus::Default:
        this->setVisible(false);
        break;
    case UpdatesStatus::UpdatesAvailable:
        m_controlWidget->showUpdateProcess(false);
        setVisible(true);
        break;
    case UpdatesStatus::Downloading:
        m_controlWidget-> setButtonStatus(ButtonStatus::pause);
        m_controlWidget->showUpdateProcess(true);
        m_controlWidget->setProgressType(UpdateDProgressType::Download);
        setProgress(m_progressVlaue);
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
        m_controlWidget->setButtonStatus(ButtonStatus::invalid);
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
        m_controlWidget->setButtonStatus(ButtonStatus::invalid);
        m_controlWidget->setCtrlButtonEnabled(false);
        break;
    case UpdatesStatus::UpdateSucceeded:
        m_controlWidget->setProgressType(UpdateDProgressType::Install);
        m_controlWidget->setButtonStatus(ButtonStatus::invalid);
        this->setVisible(false);
        break;
    case UpdatesStatus::UpdateFailed:
        m_controlWidget->showUpdateProcess(true);
        setUpdateFailedInfo();
        m_controlWidget->showButton(true);
        m_controlWidget->setCtrlButtonEnabled(true);
        m_controlWidget->setButtonStatus(ButtonStatus::retry);
        break;
    case UpdatesStatus::NeedRestart:
        m_controlWidget->setProgressText(tr("The newest system installed, restart to take effect"));
        m_controlWidget->showButton(false);
        break;
    case UpdatesStatus::WaitForRecoveryBackup:
        m_controlWidget->showUpdateProcess(true);
        m_controlWidget->setProgressText(tr("Waiting"));
        m_controlWidget->setButtonStatus(ButtonStatus::invalid);
        break;
    case UpdatesStatus::RecoveryBackingup:
        m_controlWidget->showUpdateProcess(true);
        m_controlWidget->setButtonStatus(ButtonStatus::invalid);
        m_controlWidget->setProgressType(UpdateDProgressType::Backup);
        m_controlWidget->setProgressText(tr("Backing up"));
        break;
    case UpdatesStatus::RecoveryBackingSuccessed:
        m_controlWidget->showUpdateProcess(true);
        setProgressVlaue(1.0);
        m_controlWidget->setProgressType(UpdateDProgressType::Backup);
        m_controlWidget->setButtonStatus(ButtonStatus::invalid);
        break;
    case UpdatesStatus::RecoveryBackupFailed:
        m_controlWidget->showUpdateProcess(true);
        m_controlWidget->setProgressType(UpdateDProgressType::Backup);
        m_controlWidget->setProgressText(tr("System backup failed"));
        m_controlWidget->showButton(true);
        m_controlWidget->setCtrlButtonEnabled(true);
        m_controlWidget->setButtonStatus(ButtonStatus::retry);
        break;
    case UpdatesStatus::RecoveryBackupFailedDiskFull:
        m_controlWidget->showUpdateProcess(true);
        m_controlWidget->setProgressType(UpdateDProgressType::Backup);
        m_controlWidget->setProgressText(tr("System backup failed, space is full"));
        m_controlWidget->showButton(true);
        m_controlWidget->setCtrlButtonEnabled(true);
        m_controlWidget->setButtonStatus(ButtonStatus::retry);
        break;
    default:
        qDebug() << "unknown status!!!";
        break;
    }

    // 默认状态 不用刷新页面按钮
    if (m_status != UpdatesStatus::Default) {
        Q_EMIT requestRefreshWidget();
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

    QString value = updateItemInfo->updateTime().isEmpty() ? "" : tr("Release date: ") + updateItemInfo->updateTime();
    m_controlWidget->setDate(value);
    const QString &systemVersionType = IsServerSystem ? tr("Server") : tr("Desktop");
    QString version;
    if (!updateItemInfo->availableVersion().isEmpty()) {
        QString avaVersion = updateItemInfo->availableVersion();
        QString tmpVersion = avaVersion;
        if (IsProfessionalSystem)
            tmpVersion = avaVersion.replace(avaVersion.length() - 1, 1, '0'); // 替换版本号的最后一位为‘0‘
        version = tr("Version") + ": " + systemVersionType + tmpVersion;
    }
    m_controlWidget->setVersion(version);
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

UpdateErrorType UpdateSettingItem::getUpdateJobErrorMessage() const
{
    return m_updateJobErrorMessage;
}

void UpdateSettingItem::setUpdateJobErrorMessage(const UpdateErrorType &updateJobErrorMessage)
{
    m_updateJobErrorMessage = updateJobErrorMessage;
}

void UpdateSettingItem::setUpdateFailedInfo()
{
    QString failedInfo = "";
    QString failedTips = "";
    UpdateErrorType errorType = getUpdateJobErrorMessage();
    if (m_UpdateErrorInfoMap.contains(errorType)) {
        Error_Info info = m_UpdateErrorInfoMap.value(errorType);
        failedInfo = info.errorMessage;
        failedTips = info.errorTips;
    }

    m_controlWidget->setProgressText(failedInfo, failedTips);
}

double UpdateSettingItem::getProgressVlaue() const
{
    return m_progressVlaue;
}

void UpdateSettingItem::setProgressVlaue(double progressVlaue)
{
    if (progressVlaue < 0.0 || progressVlaue > 1.0)
        return;

    m_progressVlaue = progressVlaue;
    setProgress(progressVlaue);
}

qlonglong UpdateSettingItem::updateSize() const
{
    return m_updateSize;
}

void UpdateSettingItem::setUpdateSize(const qlonglong &updateSize)
{
    if (m_updateSize != updateSize) {
        m_updateSize = updateSize;
        Q_EMIT requestRefreshSize();
        Q_EMIT requestRefreshWidget();
    }
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
    m_controlWidget->setButtonStatus(ButtonStatus::invalid);

    if (m_updateJobErrorMessage == UpdateErrorType::DpkgInterrupted) {
        Q_EMIT requestFixError(m_classifyUpdateType, "dpkgInterrupted");
        return;
    }

    if (m_updateJobErrorMessage == UpdateErrorType::DeependenciesBrokenError) {
        Q_EMIT requestFixError(m_classifyUpdateType, "dependenciesBroken");
        return;
    }

    onStartUpdate();
}
