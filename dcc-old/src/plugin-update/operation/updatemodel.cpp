// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "updatemodel.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(DdcUpdateModel, "dcc-update-model")

DownloadInfo::DownloadInfo(const qlonglong &downloadSize,
                           const QList<AppUpdateInfo> &appInfos,
                           QObject *parent)
    : QObject(parent)
    , m_downloadSize(downloadSize)
    , m_downloadProgress(0)
    , m_appInfos(appInfos)
{
}

void DownloadInfo::setDownloadProgress(double downloadProgress)
{
    if (compareDouble(m_downloadProgress, downloadProgress)) {
        m_downloadProgress = downloadProgress;
        Q_EMIT downloadProgressChanged(downloadProgress);
    }
}

UpdateModel::UpdateModel(QObject *parent)
    : QObject(parent)
    , m_status(UpdatesStatus::Default)
    , m_systemUpdateStatus(UpdatesStatus::Default)
    , m_safeUpdateStatus(UpdatesStatus::Default)
    , m_unkonowUpdateStatus(UpdatesStatus::Default)
    , m_downloadInfo(nullptr)
    , m_systemUpdateInfo(nullptr)
    , m_safeUpdateInfo(nullptr)
    , m_unknownUpdateInfo(nullptr)
    , m_updateProgress(0.0)
    , m_upgradeProgress(0.0)
    , m_lowBattery(false)
    , m_netselectExist(false)
    , m_autoCleanCache(false)
    , m_autoDownloadUpdates(false)
    , m_autoInstallUpdates(false)
    , m_autoInstallUpdateType(0)
    , m_backupUpdates(true)
    , m_autoCheckUpdates(false)
    , m_updateMode(0)
    , m_autoCheckSecureUpdates(false)
    , m_autoCheckSystemUpdates(false)
    , m_autoCheckAppUpdates(false)
    , m_autoCheckThirdpartyUpdates(false)
    , m_updateNotify(false)
    , m_mirrorId(QString())
    , m_systemVersionInfo(QString())
    , m_metaEnum(QMetaEnum::fromType<ModelUpdatesStatus>())
    , m_bSystemActivation(UiActiveState::Unknown)
    , m_lastCheckUpdateTime(QString())
    , m_autoCheckUpdateCircle(0)
    , m_isUpdatablePackages(false)
    , m_testingChannelStatus(TestingChannelStatus::DeActive)
{

    qRegisterMetaType<TestingChannelStatus>("TestingChannelStatus");
}

UpdateModel::~UpdateModel()
{
    deleteUpdateInfo(m_systemUpdateInfo);
    deleteUpdateInfo(m_safeUpdateInfo);
    deleteUpdateInfo(m_unknownUpdateInfo);
}

DownloadInfo *UpdateModel::downloadInfo() const
{
    return m_downloadInfo;
}

UpdateItemInfo *UpdateModel::systemDownloadInfo() const
{
    return m_systemUpdateInfo;
}

UpdateItemInfo *UpdateModel::safeDownloadInfo() const
{
    return m_safeUpdateInfo;
}

UpdateItemInfo *UpdateModel::unknownDownloadInfo() const
{
    return m_unknownUpdateInfo;
}

QMap<ClassifyUpdateType, UpdateItemInfo *> UpdateModel::allDownloadInfo() const
{
    return m_allUpdateInfos;
}

void UpdateModel::setDownloadInfo(DownloadInfo *downloadInfo)
{
    if (m_downloadInfo) {
        m_downloadInfo->deleteLater();
        m_downloadInfo = nullptr;
    }

    m_downloadInfo = downloadInfo;
    Q_EMIT downloadInfoChanged(downloadInfo);
}

void UpdateModel::setSystemDownloadInfo(UpdateItemInfo *updateItemInfo)
{
    if (!updateItemInfo) {
        return;
    }

    deleteUpdateInfo(m_systemUpdateInfo);

    m_systemUpdateInfo = updateItemInfo;
    connect(m_systemUpdateInfo,
            &UpdateItemInfo::downloadProgressChanged,
            this,
            &UpdateModel::systemUpdateProgressChanged);
    connect(m_systemUpdateInfo,
            &UpdateItemInfo::downloadSizeChanged,
            this,
            &UpdateModel::systemUpdateDownloadSizeChanged);

    Q_EMIT systemUpdateInfoChanged(updateItemInfo);
}

void UpdateModel::setSafeDownloadInfo(UpdateItemInfo *updateItemInfo)
{
    if (!updateItemInfo) {
        return;
    }

    deleteUpdateInfo(m_safeUpdateInfo);
    m_safeUpdateInfo = updateItemInfo;
    connect(m_safeUpdateInfo,
            &UpdateItemInfo::downloadProgressChanged,
            this,
            &UpdateModel::safeUpdateProgressChanged);
    connect(m_safeUpdateInfo,
            &UpdateItemInfo::downloadSizeChanged,
            this,
            &UpdateModel::safeUpdateDownloadSizeChanged);

    Q_EMIT safeUpdateInfoChanged(updateItemInfo);
}

void UpdateModel::setUnknownDownloadInfo(UpdateItemInfo *updateItemInfo)
{
    if (!updateItemInfo) {
        return;
    }

    deleteUpdateInfo(m_unknownUpdateInfo);
    m_unknownUpdateInfo = updateItemInfo;
    connect(m_unknownUpdateInfo,
            &UpdateItemInfo::downloadProgressChanged,
            this,
            &UpdateModel::unkonowUpdateProgressChanged);
    connect(m_unknownUpdateInfo,
            &UpdateItemInfo::downloadSizeChanged,
            this,
            &UpdateModel::unkonowUpdateDownloadSizeChanged);

    Q_EMIT unknownUpdateInfoChanged(updateItemInfo);
}

void UpdateModel::setAllDownloadInfo(QMap<ClassifyUpdateType, UpdateItemInfo *> &allUpdateInfoInfo)
{
    m_allUpdateInfos = allUpdateInfoInfo;

    setSystemDownloadInfo(allUpdateInfoInfo.value(ClassifyUpdateType::SystemUpdate));
    setSafeDownloadInfo(allUpdateInfoInfo.value(ClassifyUpdateType::SecurityUpdate));
    setUnknownDownloadInfo(allUpdateInfoInfo.value(ClassifyUpdateType::UnknownUpdate));
}

bool UpdateModel::lowBattery() const
{
    return m_lowBattery;
}

void UpdateModel::setLowBattery(bool lowBattery)
{
    if (lowBattery != m_lowBattery) {
        m_lowBattery = lowBattery;
        Q_EMIT lowBatteryChanged(lowBattery);
    }
}

bool UpdateModel::autoDownloadUpdates() const
{
    return m_autoDownloadUpdates;
}

void UpdateModel::setAutoDownloadUpdates(bool autoDownloadUpdates)
{
    if (m_autoDownloadUpdates != autoDownloadUpdates) {
        m_autoDownloadUpdates = autoDownloadUpdates;
        Q_EMIT autoDownloadUpdatesChanged(autoDownloadUpdates);
    }
}

UpdatesStatus UpdateModel::status() const
{
    return m_status;
}

void UpdateModel::setStatus(const UpdatesStatus &status)
{
    if (getIsRecoveryBackingup(status))
        return;

    if (m_status != status) {
        m_status = status;
        Q_EMIT statusChanged(status);
    }
}

void UpdateModel::setStatus(const UpdatesStatus &status, int line)
{
    qCDebug(DdcUpdateModel) << " from work set status : " << m_metaEnum.valueToKey(status)
                            << " , set place in work line : " << line;
    setStatus(status);
}

double UpdateModel::upgradeProgress() const
{
    return m_upgradeProgress;
}

void UpdateModel::setUpgradeProgress(double upgradeProgress)
{
    if (compareDouble(m_upgradeProgress, upgradeProgress)) {
        m_upgradeProgress = upgradeProgress;
        Q_EMIT upgradeProgressChanged(upgradeProgress);
    }
}

bool UpdateModel::autoCleanCache() const
{
    return m_autoCleanCache;
}

void UpdateModel::setAutoCleanCache(bool autoCleanCache)
{
    if (m_autoCleanCache == autoCleanCache)
        return;

    m_autoCleanCache = autoCleanCache;
    Q_EMIT autoCleanCacheChanged(autoCleanCache);
}

double UpdateModel::updateProgress() const
{
    return m_updateProgress;
}

void UpdateModel::setUpdateProgress(double updateProgress)
{
    if (compareDouble(m_updateProgress, updateProgress)) {
        m_updateProgress = updateProgress;
        Q_EMIT updateProgressChanged(updateProgress);
    }
}

bool UpdateModel::netselectExist() const
{
    return m_netselectExist;
}

void UpdateModel::setNetselectExist(bool netselectExist)
{
    if (m_netselectExist == netselectExist) {
        return;
    }

    m_netselectExist = netselectExist;

    Q_EMIT netselectExistChanged(netselectExist);
}

void UpdateModel::setAutoCheckUpdates(bool autoCheckUpdates)
{
    if (autoCheckUpdates == m_autoCheckUpdates)
        return;

    m_autoCheckUpdates = autoCheckUpdates;

    Q_EMIT autoCheckUpdatesChanged(autoCheckUpdates);
}

void UpdateModel::setUpdateMode(qulonglong updateMode)
{
    qCDebug(DdcUpdateModel) << Q_FUNC_INFO << "get UpdateMode from dbus:" << updateMode;

    if (m_updateMode == updateMode) {
        return;
    }

    m_updateMode = updateMode;

    setAutoCheckSystemUpdates(m_updateMode & ClassifyUpdateType::SystemUpdate);
    setAutoCheckAppUpdates(m_updateMode & ClassifyUpdateType::AppStoreUpdate);
    setAutoCheckSecureUpdates(m_updateMode & ClassifyUpdateType::SecurityUpdate);
    setAutoCheckThirdpartyUpdates(m_updateMode & ClassifyUpdateType::UnknownUpdate);
}

void UpdateModel::setAutoCheckSecureUpdates(bool autoCheckSecureUpdates)
{
    if (autoCheckSecureUpdates == m_autoCheckSecureUpdates)
        return;

    m_autoCheckSecureUpdates = autoCheckSecureUpdates;

    Q_EMIT autoCheckSecureUpdatesChanged(autoCheckSecureUpdates);
}

void UpdateModel::setAutoCheckSystemUpdates(bool autoCheckSystemUpdates)
{
    if (autoCheckSystemUpdates == m_autoCheckSystemUpdates)
        return;

    m_autoCheckSystemUpdates = autoCheckSystemUpdates;

    Q_EMIT autoCheckSystemUpdatesChanged(autoCheckSystemUpdates);
}

void UpdateModel::setAutoCheckAppUpdates(bool autoCheckAppUpdates)
{
    if (autoCheckAppUpdates == m_autoCheckAppUpdates)
        return;

    m_autoCheckAppUpdates = autoCheckAppUpdates;

    Q_EMIT autoCheckAppUpdatesChanged(autoCheckAppUpdates);
}

void UpdateModel::setSystemVersionInfo(const QString &systemVersionInfo)
{
    if (m_systemVersionInfo == systemVersionInfo)
        return;

    m_systemVersionInfo = systemVersionInfo;

    Q_EMIT systemVersionChanged(systemVersionInfo);
}

void UpdateModel::setSystemActivation(const UiActiveState &systemactivation)
{
    if (m_bSystemActivation == systemactivation) {
        return;
    }
    m_bSystemActivation = systemactivation;

    Q_EMIT systemActivationChanged(systemactivation);
}

void UpdateModel::isUpdatablePackages(bool isUpdatablePackages)
{
    if (m_isUpdatablePackages == isUpdatablePackages)
        return;

    m_isUpdatablePackages = isUpdatablePackages;
    Q_EMIT updatablePackagesChanged(isUpdatablePackages);
}

// 判断当前是否正在备份中，若正在备份则不能再设置其他状态，直到备份有结果了才能继续设置其他状态
bool UpdateModel::getIsRecoveryBackingup(UpdatesStatus state) const
{
    bool ret = true;

    if (m_status == UpdatesStatus::RecoveryBackingup) {
        if (state == UpdatesStatus::RecoveryBackingSuccessed
            || state == UpdatesStatus::RecoveryBackupFailed
            || state == UpdatesStatus::RecoveryBackupFailedDiskFull) {
            ret = false;
            qCDebug(DdcUpdateModel) << " Backing up End ! , state : " << state;
        } else {
            qCDebug(DdcUpdateModel) << " Now is Backing up , can't set other status. Please wait..."
                                    << m_metaEnum.valueToKey(state);
        }
    } else {
        ret = false;
    }

    return ret;
}

void UpdateModel::setLastCheckUpdateTime(const QString &lastTime)
{
    qCDebug(DdcUpdateModel) << "Last check time:" << lastTime;
    m_lastCheckUpdateTime = lastTime.left(QString("0000-00-00 00:00:00").size());
}

void UpdateModel::setHistoryAppInfos(const QList<AppUpdateInfo> &infos)
{
    m_historyAppInfos = infos;
}

void UpdateModel::setAutoCheckUpdateCircle(const int interval)
{
    m_autoCheckUpdateCircle = interval;
}

bool UpdateModel::enterCheckUpdate()
{
    qCDebug(DdcUpdateModel) << "last update time:" << m_lastCheckUpdateTime
                            << "check circle:" << m_autoCheckUpdateCircle;
    return QDateTime::fromString(m_lastCheckUpdateTime, "yyyy-MM-dd hh:mm:ss")
                   .secsTo(QDateTime::currentDateTime())
            > m_autoCheckUpdateCircle * 3600;
}

void UpdateModel::setUpdateNotify(const bool notify)
{
    if (m_updateNotify == notify) {
        return;
    }

    m_updateNotify = notify;

    Q_EMIT updateNotifyChanged(notify);
}

UpdatesStatus UpdateModel::getUnkonowUpdateStatus() const
{
    return m_unkonowUpdateStatus;
}

void UpdateModel::setUnkonowUpdateStatus(const UpdatesStatus &unkonowUpdateStatus)
{
    if (m_unkonowUpdateStatus != unkonowUpdateStatus) {
        m_unkonowUpdateStatus = unkonowUpdateStatus;
        Q_EMIT unkonowUpdateStatusChanged(unkonowUpdateStatus);
    }
}

UpdatesStatus UpdateModel::getSafeUpdateStatus() const
{
    return m_safeUpdateStatus;
}

void UpdateModel::setSafeUpdateStatus(const UpdatesStatus &safeUpdateStatus)
{
    if (m_safeUpdateStatus != safeUpdateStatus) {
        m_safeUpdateStatus = safeUpdateStatus;
        Q_EMIT safeUpdateStatusChanged(safeUpdateStatus);
    }
}

UpdatesStatus UpdateModel::getSystemUpdateStatus() const
{
    return m_systemUpdateStatus;
}

void UpdateModel::setSystemUpdateStatus(const UpdatesStatus &systemUpdateStatus)
{
    if (m_systemUpdateStatus != systemUpdateStatus) {
        m_systemUpdateStatus = systemUpdateStatus;
        Q_EMIT systemUpdateStatusChanged(systemUpdateStatus);
    }
}

void UpdateModel::setClassifyUpdateTypeStatus(ClassifyUpdateType type, UpdatesStatus status)
{
    switch (type) {
    case ClassifyUpdateType::SystemUpdate:
        setSystemUpdateStatus(status);
        break;
    case ClassifyUpdateType::SecurityUpdate:
        setSafeUpdateStatus(status);
        break;
    case ClassifyUpdateType::UnknownUpdate:
        setUnkonowUpdateStatus(status);
        break;
    default:
        break;
    }
}

void UpdateModel::setAllClassifyUpdateStatus(UpdatesStatus status)
{
    setSystemUpdateStatus(status);
    setSafeUpdateStatus(status);
    setUnkonowUpdateStatus(status);
}

void UpdateModel::deleteUpdateInfo(UpdateItemInfo *updateItemInfo)
{
    if (updateItemInfo != nullptr) {
        updateItemInfo->deleteLater();
    }
}

bool UpdateModel::getAutoInstallUpdates() const
{
    return m_autoInstallUpdates;
}

void UpdateModel::setAutoInstallUpdates(bool autoInstallUpdates)
{
    if (m_autoInstallUpdates != autoInstallUpdates) {
        m_autoInstallUpdates = autoInstallUpdates;
        Q_EMIT autoInstallUpdatesChanged(autoInstallUpdates);
    }
}

bool UpdateModel::getBackupUpdates() const
{
    return m_backupUpdates;
}

void UpdateModel::setBackupUpdates(bool backupsUpdates)
{
    if (m_backupUpdates != backupsUpdates) {
        m_backupUpdates = backupsUpdates;
        Q_EMIT backupUpdatesChanged(backupsUpdates);
    }
}

quint64 UpdateModel::getAutoInstallUpdateType() const
{
    return m_autoInstallUpdateType;
}

void UpdateModel::setAutoInstallUpdateType(const quint64 &autoInstallUpdateType)
{
    if (m_autoInstallUpdateType != autoInstallUpdateType) {
        m_autoInstallUpdateType = autoInstallUpdateType;
        Q_EMIT autoInstallUpdateTypeChanged(autoInstallUpdateType);
    }
}

QMap<ClassifyUpdateType, UpdateItemInfo *> UpdateModel::getAllUpdateInfos() const
{
    return m_allUpdateInfos;
}

void UpdateModel::setAllUpdateInfos(
        const QMap<ClassifyUpdateType, UpdateItemInfo *> &allUpdateInfos)
{
    m_allUpdateInfos = allUpdateInfos;
}

UpdateJobErrorMessage UpdateModel::getSystemUpdateJobError() const
{
    return m_systemUpdateJobError;
}

void UpdateModel::setSystemUpdateJobError(const UpdateJobErrorMessage &systemUpdateJobError)
{
    m_systemUpdateJobError = systemUpdateJobError;
}

UpdateJobErrorMessage UpdateModel::getSafeUpdateJobError() const
{
    return m_safeUpdateJobError;
}

void UpdateModel::setSafeUpdateJobError(const UpdateJobErrorMessage &safeUpdateJobError)
{
    m_safeUpdateJobError = safeUpdateJobError;
}

UpdateJobErrorMessage UpdateModel::getUnkonwUpdateJobError() const
{
    return m_UnkonwUpdateJobError;
}

void UpdateModel::setUnkonwUpdateJobError(const UpdateJobErrorMessage &UnkonwUpdateJobError)
{
    m_UnkonwUpdateJobError = UnkonwUpdateJobError;
}

void UpdateModel::setClassityUpdateJonError(ClassifyUpdateType type, UpdateErrorType errorType)
{
    if (m_updateErrorTypeMap.contains(type)) {
        m_updateErrorTypeMap.remove(type);
    }
    m_updateErrorTypeMap.insert(type, errorType);

    Q_EMIT classityUpdateJobErrorChanged(type, errorType);
}

QMap<ClassifyUpdateType, UpdateErrorType> UpdateModel::getUpdateErrorTypeMap() const
{
    return m_updateErrorTypeMap;
}

bool UpdateModel::getAutoCheckThirdpartyUpdates() const
{
    return m_autoCheckThirdpartyUpdates;
}

void UpdateModel::setAutoCheckThirdpartyUpdates(bool autoCheckThirdpartyUpdates)
{
    if (m_autoCheckThirdpartyUpdates != autoCheckThirdpartyUpdates) {
        m_autoCheckThirdpartyUpdates = autoCheckThirdpartyUpdates;
        Q_EMIT autoCheckThirdpartyUpdatesChanged(m_autoCheckThirdpartyUpdates);
    }
}

QString UpdateModel::commitSubmissionTime()
{
    // 显示时间，⼗位时间度为秒;
    QString currentTime = QByteArray::number(QDateTime::currentDateTime().toTime_t());
    return currentTime;
}

QString UpdateModel::systemVersion()
{
    QString systemVer = QString("uos-%1-%2-%3")
                                .arg(DSysInfo::majorVersion())
                                .arg(DSysInfo::minorVersion())
                                .arg(DSysInfo::buildVersion());
    return systemVer;
}

int32_t UpdateModel::submissionType()
{
    // 控制中心 默认 0
    // 0系统提交 1⽤户提交 2装器提交
    return 0;
}

QString UpdateModel::UUID()
{
    return "02eb924f-4f35-4880-b839-096c3a65f525";
}

QString UpdateModel::utcDateTime2LocalDate(const QString &utcDateTime)
{
    if (utcDateTime.isEmpty())
        return "";

    QDateTime dateTime = QDateTime::fromString(utcDateTime, "yyyy-MM-ddTHH:mm:ss+08:00");
    if (!dateTime.isValid())
        return "";

    return dateTime.toLocalTime().toString("yyyy-MM-dd");
}

UpdatesStatus UpdateModel::getClassifyUpdateStatus(ClassifyUpdateType type)
{
    UpdatesStatus status = UpdatesStatus::Default;
    switch (type) {
    case ClassifyUpdateType::SystemUpdate:
        status = getSystemUpdateStatus();
        break;
    case ClassifyUpdateType::SecurityUpdate:
        status = getSafeUpdateStatus();
        break;
    case ClassifyUpdateType::UnknownUpdate:
        status = getUnkonowUpdateStatus();
        break;
    default:
        break;
    }
    return status;
}

TestingChannelStatus UpdateModel::getTestingChannelStatus() const
{
    return m_testingChannelStatus;
}

void UpdateModel::setTestingChannelStatus(const TestingChannelStatus &status)
{
    m_testingChannelStatus = status;
    Q_EMIT TestingChannelStatusChanged(m_testingChannelStatus);
}

void UpdateModel::setMirrorInfos(const MirrorInfoList &list)
{
    m_mirrorList = list;
}

MirrorInfo UpdateModel::defaultMirror() const
{
    QList<MirrorInfo>::const_iterator it = m_mirrorList.begin();
    for (; it != m_mirrorList.end(); ++it) {
        if ((*it).m_id == m_mirrorId) {
            return *it;
        }
    }

    return m_mirrorList.at(0);
}

void UpdateModel::setDefaultMirror(const QString &mirrorId)
{
    if (mirrorId == "")
        return;

    m_mirrorId = mirrorId;

    QList<MirrorInfo>::iterator it = m_mirrorList.begin();
    for (; it != m_mirrorList.end(); ++it) {
        if ((*it).m_id == mirrorId) {
            Q_EMIT defaultMirrorChanged(*it);
        }
    }
}

void UpdateModel::setSmartMirrorSwitch(bool smartMirrorSwitch)
{
    if (m_smartMirrorSwitch == smartMirrorSwitch)
        return;

    m_smartMirrorSwitch = smartMirrorSwitch;

    Q_EMIT smartMirrorSwitchChanged(smartMirrorSwitch);
}

void UpdateModel::setMirrorSpeedInfo(const QMap<QString, int> &mirrorSpeedInfo)
{
    m_mirrorSpeedInfo = mirrorSpeedInfo;

    if (mirrorSpeedInfo.keys().length())
        Q_EMIT mirrorSpeedInfoAvailable(mirrorSpeedInfo);
}
