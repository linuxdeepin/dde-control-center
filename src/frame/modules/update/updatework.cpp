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

#include "updatework.h"
#include "window/utils.h"
#include "widgets/utils.h"
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QApplication>
#include <QMutexLocker>
#include <vector>

#define MIN_NM_ACTIVE 50
#define UPDATE_PACKAGE_SIZE 0
using namespace DCC_NAMESPACE;

const QString ChangeLogFile = "/usr/share/deepin/release-note/UpdateInfo.json";
const QString ChangeLogDic = "/usr/share/deepin/";

// 系统补丁标识
const QString DDEId = "dde";

namespace dcc {
namespace update {
static int TestMirrorSpeedInternal(const QString &url, QPointer<QObject> baseObject)
{
    if (!baseObject || QCoreApplication::closingDown()) {
        return -1;
    }

    QStringList args;
    args << url << "-s" << "1";

    QProcess process;
    process.start("netselect", args);

    if (!process.waitForStarted()) {
        return 10000;
    }

    do {
        if (!baseObject || QCoreApplication::closingDown()) {
            process.kill();
            process.terminate();
            process.waitForFinished(1000);

            return -1;
        }

        if (process.waitForFinished(500))
            break;
    } while (process.state() == QProcess::Running);

    const QString output = process.readAllStandardOutput().trimmed();
    const QStringList result = output.split(' ');

    qDebug() << "speed of url" << url << "is" << result.first();

    if (!result.first().isEmpty()) {
        return result.first().toInt();
    }

    return 10000;
}

UpdateWorker::UpdateWorker(UpdateModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_checkUpdateJob(nullptr)
    , m_sysUpdateDownloadJob(nullptr)
    , m_safeUpdateDownloadJob(nullptr)
    , m_unknownUpdateDownloadJob(nullptr)
    , m_sysUpdateInstallJob(nullptr)
    , m_safeUpdateInstallJob(nullptr)
    , m_unknownUpdateInstallJob(nullptr)
    , m_lastoresessionHelper(nullptr)
    , m_updateInter(nullptr)
    , m_managerInter(nullptr)
    , m_powerInter(nullptr)
    , m_powerSystemInter(nullptr)
    , m_networkInter(nullptr)
    , m_smartMirrorInter(nullptr)
    , m_abRecoveryInter(nullptr)
    , m_iconTheme(nullptr)
    , m_onBattery(true)
    , m_batteryPercentage(0.0)
    , m_batterySystemPercentage(0.0)
    , m_jobPath("")
    , m_downloadSize(0)
    , m_iconThemeState("")
    , m_backupStatus(BackupStatus::NoBackup)
    , m_backupingClassifyType(ClassifyUpdateType::Invalid)
    , m_releaseNoteJobStatus("")
    , m_releaseNoteUpdated(false)
    , m_fileSystemWatcher(new QFileSystemWatcher(this))
{

}

UpdateWorker::~UpdateWorker()
{
    deleteJob(m_sysUpdateDownloadJob);
    deleteJob(m_sysUpdateInstallJob);
    deleteJob(m_safeUpdateDownloadJob);
    deleteJob(m_safeUpdateInstallJob);
    deleteJob(m_unknownUpdateDownloadJob);
    deleteJob(m_unknownUpdateInstallJob);
    deleteJob(m_checkUpdateJob);
    deleteJob(m_releaseNoteInstallJob);
    if (m_fileSystemWatcher != nullptr) {
        delete m_fileSystemWatcher;
        m_fileSystemWatcher = nullptr;
    }
}

void UpdateWorker::init()
{
    qRegisterMetaType<UpdatesStatus>("UpdatesStatus");
    qRegisterMetaType<UiActiveState>("UiActiveState");

    m_lastoresessionHelper = new LastoressionHelper("com.deepin.LastoreSessionHelper", "/com/deepin/LastoreSessionHelper", QDBusConnection::sessionBus(), this);
    m_updateInter = new UpdateInter("com.deepin.lastore", "/com/deepin/lastore", QDBusConnection::systemBus(), this);
    m_managerInter = new ManagerInter("com.deepin.lastore", "/com/deepin/lastore", QDBusConnection::systemBus(), this);
    m_powerInter = new PowerInter("com.deepin.daemon.Power", "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this);
    m_powerSystemInter = new PowerSystemInter("com.deepin.system.Power", "/com/deepin/system/Power", QDBusConnection::systemBus(), this);
    m_networkInter = new Network("com.deepin.daemon.Network", "/com/deepin/daemon/Network", QDBusConnection::sessionBus(), this);
    m_smartMirrorInter = new SmartMirrorInter("com.deepin.lastore.Smartmirror", "/com/deepin/lastore/Smartmirror", QDBusConnection::systemBus(), this);
    m_abRecoveryInter = new RecoveryInter("com.deepin.ABRecovery", "/com/deepin/ABRecovery", QDBusConnection::systemBus(), this);
    m_iconTheme = new Appearance("com.deepin.daemon.Appearance", "/com/deepin/daemon/Appearance", QDBusConnection::sessionBus(), this);

    m_managerInter->setSync(false);
    m_updateInter->setSync(false);
    m_powerInter->setSync(false);
    m_powerSystemInter->setSync(false);
    m_lastoresessionHelper->setSync(false);
    m_smartMirrorInter->setSync(false, false);
    m_iconTheme->setSync(false);

    QString sVersion = QString("%1 %2").arg(DSysInfo::uosProductTypeName()).arg(DSysInfo::majorVersion());
    if (!IsServerSystem)
        sVersion.append(" " + DSysInfo::uosEditionName());
    m_model->setSystemVersionInfo(sVersion);

    connect(m_managerInter, &ManagerInter::JobListChanged, this, &UpdateWorker::onJobListChanged);
    connect(m_managerInter, &ManagerInter::AutoCleanChanged, m_model, &UpdateModel::setAutoCleanCache);

    connect(m_updateInter, &__Updater::AutoDownloadUpdatesChanged, m_model, &UpdateModel::setAutoDownloadUpdates);
    connect(m_updateInter, &__Updater::AutoInstallUpdatesChanged, m_model, &UpdateModel::setAutoInstallUpdates);
    connect(m_updateInter, &__Updater::AutoInstallUpdateTypeChanged, m_model, &UpdateModel::setAutoInstallUpdateType);
    connect(m_updateInter, &__Updater::MirrorSourceChanged, m_model, &UpdateModel::setDefaultMirror);
    connect(m_updateInter, &UpdateInter::AutoCheckUpdatesChanged, m_model, &UpdateModel::setAutoCheckUpdates);
    connect(m_managerInter, &ManagerInter::UpdateModeChanged, m_model, &UpdateModel::setUpdateMode);
    connect(m_updateInter, &UpdateInter::UpdateNotifyChanged, m_model, &UpdateModel::setUpdateNotify);
    connect(m_updateInter, &UpdateInter::ClassifiedUpdatablePackagesChanged, this, &UpdateWorker::onClassifiedUpdatablePackagesChanged);

    connect(m_powerInter, &__Power::OnBatteryChanged, this, &UpdateWorker::setOnBattery);
    connect(m_powerInter, &__Power::BatteryPercentageChanged, this, &UpdateWorker::setBatteryPercentage);

    // connect(m_powerSystemInter, &__SystemPower::BatteryPercentageChanged, this, &UpdateWorker::setSystemBatteryPercentage);
    if (IsCommunitySystem) {
        connect(m_smartMirrorInter, &SmartMirrorInter::EnableChanged, m_model, &UpdateModel::setSmartMirrorSwitch);
        connect(m_smartMirrorInter, &SmartMirrorInter::serviceValidChanged, this, &UpdateWorker::onSmartMirrorServiceIsValid);
        connect(m_smartMirrorInter, &SmartMirrorInter::serviceStartFinished, this, [ = ] {
            QTimer::singleShot(100, this, [ = ] {
                m_model->setSmartMirrorSwitch(m_smartMirrorInter->enable());
            });
        }, Qt::UniqueConnection);
    }

    connect(m_abRecoveryInter, &RecoveryInter::JobEnd, this, &UpdateWorker::onRecoveryBackupFinshed);
    connect(m_abRecoveryInter, &RecoveryInter::BackingUpChanged, m_model, &UpdateModel::setRecoverBackingUp);
    connect(m_abRecoveryInter, &RecoveryInter::ConfigValidChanged, m_model, &UpdateModel::setRecoverConfigValid);
    connect(m_abRecoveryInter, &RecoveryInter::RestoringChanged, m_model, &UpdateModel::setRecoverRestoring);
    //图片主题
    connect(m_iconTheme, &Appearance::IconThemeChanged, this, &UpdateWorker::onIconThemeChanged);

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    connect(m_lastoresessionHelper, &LastoressionHelper::SourceCheckEnabledChanged, m_model, &UpdateModel::setSourceCheck);
#endif
}

void UpdateWorker::licenseStateChangeSlot()
{
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
    connect(watcher, &QFutureWatcher<void>::finished, watcher, &QFutureWatcher<void>::deleteLater);

    QFuture<void> future = QtConcurrent::run(this, &UpdateWorker::getLicenseState);
    watcher->setFuture(future);
}

void UpdateWorker::getLicenseState()
{
    if (DSysInfo::DeepinDesktop == DSysInfo::deepinType()) {
        m_model->setSystemActivation(UiActiveState::Authorized);
        return;
    }
    QDBusInterface licenseInfo("com.deepin.license",
                               "/com/deepin/license/Info",
                               "com.deepin.license.Info",
                               QDBusConnection::systemBus());
    if (!licenseInfo.isValid()) {
        qDebug() << "com.deepin.license error ," << licenseInfo.lastError().name();
        return;
    }
    UiActiveState reply = static_cast<UiActiveState>(licenseInfo.property("AuthorizationState").toInt());
    qDebug() << "Authorization State:" << reply;
    m_model->setSystemActivation(reply);
}

void UpdateWorker::activate()
{
#ifndef DISABLE_SYS_UPDATE_MIRRORS
    refreshMirrors();
#endif
    m_managerInter->setSync(true);
    m_updateInter->setSync(true);
    QString checkTime;
    double interval = m_updateInter->GetCheckIntervalAndTime(checkTime);
    m_model->setLastCheckUpdateTime(checkTime);
    m_model->setAutoCheckUpdateCircle(static_cast<int>(interval));

    m_model->setAutoCleanCache(m_managerInter->autoClean());
    m_model->setAutoDownloadUpdates(m_updateInter->autoDownloadUpdates());
    m_model->setAutoInstallUpdates(m_updateInter->autoInstallUpdates());
    m_model->setAutoInstallUpdateType(m_updateInter->autoInstallUpdateType());
    m_model->setAutoCheckUpdates(m_updateInter->autoCheckUpdates());
    m_model->setUpdateMode(m_managerInter->updateMode());
    m_model->setUpdateNotify(m_updateInter->updateNotify());
    if (IsCommunitySystem) {
        m_model->setSmartMirrorSwitch(m_smartMirrorInter->enable());
        onSmartMirrorServiceIsValid(m_smartMirrorInter->isValid());
    }
#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    m_model->setSourceCheck(m_lastoresessionHelper->sourceCheckEnabled());
#endif
    m_model->setRecoverConfigValid(m_abRecoveryInter->configValid());

    setOnBattery(m_powerInter->onBattery());
    setBatteryPercentage(m_powerInter->batteryPercentage());
    // setSystemBatteryPercentage(m_powerSystemInter->batteryPercentage());

    const QList<QDBusObjectPath> jobs = m_managerInter->jobList();
    if (jobs.count() > 0) {
        for (QDBusObjectPath dBusObjectPath : jobs) {
            if (dBusObjectPath.path().contains("upgrade")) {
                qDebug() << "UpdateWorker::activate, jobs.count() == " << jobs.count();
                setUpdateInfo();
                break;
            }
        }
    }

    onJobListChanged(m_managerInter->jobList());
    m_managerInter->setSync(false);
    m_updateInter->setSync(false);

#ifndef DISABLE_SYS_UPDATE_MIRRORS
    refreshMirrors();
#endif

    licenseStateChangeSlot();

    QDBusConnection::systemBus().connect("com.deepin.license", "/com/deepin/license/Info",
                                         "com.deepin.license.Info", "LicenseStateChange",
                                         this, SLOT(licenseStateChangeSlot()));

    QFutureWatcher<QStringList> *packagesWatcher = new QFutureWatcher<QStringList>();
    connect(packagesWatcher, &QFutureWatcher<QStringList>::finished, this, [ = ] {
        QStringList updatablePackages = std::move(packagesWatcher->result());
        qDebug() << "UpdatablePackages = " << updatablePackages.count();
        m_model->isUpdatablePackages(updatablePackages.count() > UPDATE_PACKAGE_SIZE);
        packagesWatcher->deleteLater();
    });

    packagesWatcher->setFuture(QtConcurrent::run([ = ]() -> QStringList {
        QDBusInterface Interface("com.deepin.lastore", "/com/deepin/lastore",
                                 "com.deepin.lastore.Updater",
                                 QDBusConnection::systemBus());
        if (!Interface.isValid())
        {
            qDebug() << "com.deepin.license error ," << Interface.lastError().name();
            return {};
        }

        return Interface.property("UpdatablePackages").toStringList();
    }));

    QFutureWatcher<QString> *iconWatcher = new QFutureWatcher<QString>();
    connect(iconWatcher, &QFutureWatcher<QString>::finished, this, [ = ] {
        m_iconThemeState = iconWatcher->result();
        iconWatcher->deleteLater();
    });

    iconWatcher->setFuture(QtConcurrent::run([ = ] {
        bool isSync = m_iconTheme->sync();
        m_iconTheme->setSync(true);
        const QString &iconTheme = m_iconTheme->iconTheme();
        m_iconTheme->setSync(isSync);
        return iconTheme;
    }));
}

void UpdateWorker::deactivate()
{

}

void UpdateWorker::checkForUpdates()
{
    if (checkDbusIsValid()) {
        qDebug() << " checkDbusIsValid . do nothing";
        return;
    }

    QDBusPendingCall call = m_managerInter->UpdateSource();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] {
        if (!call.isError())
        {
            QDBusReply<QDBusObjectPath> reply = call.reply();
            const QString jobPath = reply.value().path();
            setCheckUpdatesJob(jobPath);
        } else
        {
            m_model->setStatus(UpdatesStatus::UpdateFailed, __LINE__);
            resetDownloadInfo();
            if (!m_checkUpdateJob.isNull()) {
                m_managerInter->CleanJob(m_checkUpdateJob->id());
            }
            qDebug() << "UpdateFailed, check for updates error: " << call.error().message();
        }
    });
}


void UpdateWorker::setUpdateInfo()
{
    m_updatePackages.clear();
    m_systemPackages.clear();
    m_safePackages.clear();
    m_unknownPackages.clear();

    qDebug() << " UpdateWorker::setUpdateInfo() ";
    m_updateInter->setSync(true);
    m_managerInter->setSync(true);

    m_updatePackages = m_updateInter->classifiedUpdatablePackages();
    m_systemPackages = m_updatePackages.value(SystemUpdateType);
    m_safePackages = m_updatePackages.value(SecurityUpdateType);
    m_unknownPackages = m_updatePackages.value(UnknownUpdateType);

    m_updateInter->setSync(false);
    m_managerInter->setSync(false);

    bool hasReleaseNote = QFile::exists(ChangeLogFile);
    if (m_systemPackages.contains("uos-release-note") || !hasReleaseNote) {
        qDebug() << "install uos-release-note";
        QEventLoop eventLoop;
        m_releaseNoteUpdated = false;
        QTimer::singleShot(30000, &eventLoop, &QEventLoop::quit);
        connect(this, &UpdateWorker::releaseNoteInstallCompleted, &eventLoop, &QEventLoop::quit);
        qDebug() << "ChangeLogFile" << ChangeLogFile;
        eventLoop.exec();
    } else {
        m_releaseNoteUpdated = true;
    }
    if (m_fileSystemWatcher != nullptr) {
        delete m_fileSystemWatcher;
        m_fileSystemWatcher = nullptr;
    }


    qDebug() << "systemUpdate packages:" <<  m_systemPackages;
    qDebug() << "safeUpdate packages:" <<  m_safePackages;
    qDebug() << "unkonowUpdate packages:" <<  m_unknownPackages;

    if (m_model->status() == UpdatesStatus::UpdateFailed) {
        qDebug() << " [UpdateWork] The status is error. Current status : " << m_model->status();
        return;
    }

    qDebug() << "uos-releasenote :" << getReleaseNoteStatus();
    int updateCount = m_systemPackages.count() + m_safePackages.count() + m_unknownPackages.count();
    if (updateCount < 1) {
        QFile file("/tmp/.dcc-update-successd");
        if (file.exists()) {
            m_model->setStatus(UpdatesStatus::NeedRestart, __LINE__);
            return;
        }
    }

    QMap<ClassifyUpdateType, UpdateItemInfo *> updateInfoMap = getAllUpdateInfo();
    m_model->setAllDownloadInfo(updateInfoMap);

    qDebug() << " UpdateWorker::setUpdateInfo: updateInfoMap.count()" << updateInfoMap.count();

    if (updateInfoMap.count() == 0) {
        m_model->setStatus(UpdatesStatus::Updated, __LINE__);
    } else {
        qDebug() << "UpdateWorker::setAppUpdateInfo: downloadSize = " << m_downloadSize;
        m_model->setStatus(UpdatesStatus::UpdatesAvailable, __LINE__);
        for (uint type = ClassifyUpdateType::SystemUpdate; type <= ClassifyUpdateType::SecurityUpdate; type++) {
            ClassifyUpdateType classifyType = uintToclassifyUpdateType(type);
            if (updateInfoMap.contains(classifyType)) {
                if (updateInfoMap.value(classifyType) != nullptr) {
                    m_downloadSize += updateInfoMap.value(classifyType) ->downloadSize();
                    if (m_model->getClassifyUpdateStatus(classifyType) != UpdatesStatus::Downloading
                            && m_model->getClassifyUpdateStatus(classifyType) != UpdatesStatus::DownloadPaused
                            && m_model->getClassifyUpdateStatus(classifyType) != UpdatesStatus::Downloaded
                            && m_model->getClassifyUpdateStatus(classifyType) != UpdatesStatus::Installing) {
                        m_model->setClassifyUpdateTypeStatus(classifyType, UpdatesStatus::UpdatesAvailable);
                    }
                }
            } else {
                m_model->setClassifyUpdateTypeStatus(classifyType, UpdatesStatus::Default);
            }
        }
    }
}

QMap<ClassifyUpdateType, UpdateItemInfo *> UpdateWorker::getAllUpdateInfo()
{
    qDebug() << "getAllUpdateInfo";
    QMap<ClassifyUpdateType, UpdateItemInfo *> resultMap;

    QMap<ClassifyUpdateType, QString> updateDailyKeyMap;
    updateDailyKeyMap.insert(ClassifyUpdateType::SystemUpdate, "systemUpdateInfo");
    updateDailyKeyMap.insert(ClassifyUpdateType::SecurityUpdate, "safeUpdateInfo");
    updateDailyKeyMap.insert(ClassifyUpdateType::UnknownUpdate, "otherUpdateInfo");

    if (m_systemPackages.count() > 0) {
        UpdateItemInfo *systemItemInfo = new UpdateItemInfo;
        systemItemInfo->setName(tr("System Updates"));
        systemItemInfo->setExplain(tr("Fixed some known bugs and security vulnerabilities"));
        setUpdateItemDownloadSize(systemItemInfo, m_systemPackages);
        resultMap.insert(ClassifyUpdateType::SystemUpdate, systemItemInfo);
    }

    if (m_safePackages.count() > 0) {
        UpdateItemInfo  *safeItemInfo = new UpdateItemInfo;
        safeItemInfo->setName(tr("Security Updates"));
        safeItemInfo->setExplain(tr("Fixed some known bugs and security vulnerabilities"));
        setUpdateItemDownloadSize(safeItemInfo, m_safePackages);
        resultMap.insert(ClassifyUpdateType::SecurityUpdate, safeItemInfo);
    }

    if (m_unknownPackages.count() > 0) {
        UpdateItemInfo *unkownItemInfo = new UpdateItemInfo;
        unkownItemInfo->setName(tr("Unknown Apps Updates"));
        setUpdateItemDownloadSize(unkownItemInfo, m_unknownPackages);
        resultMap.insert(ClassifyUpdateType::UnknownUpdate, unkownItemInfo);
    }

    if (m_releaseNoteJobStatus == "failed") {
        qWarning() << "releaseNoteStatus = " << m_releaseNoteJobStatus;
        return resultMap;
    }

    QFile logFile(ChangeLogFile);
    if (!logFile.open(QFile::ReadOnly)) {
        qDebug() << "can not find update file:" << ChangeLogFile;
        return resultMap;
    }

    QJsonParseError err_rpt;
    QJsonDocument updateInfoDoc = QJsonDocument::fromJson(logFile.readAll(), &err_rpt);
    if (err_rpt.error != QJsonParseError::NoError) {
        qDebug() << "更新日志信息JSON格式错误";
        return resultMap;
    }
    const QJsonObject &object = updateInfoDoc.object();
    for (ClassifyUpdateType type : resultMap.keys()) {
        getItemInfo(object.value(updateDailyKeyMap.value(type)), resultMap.value(type));
        qDebug() << "getAllUpdateInfo: " << updateDailyKeyMap.value(type) << " = " << object.value("otherUpdateInfo").toString();
    }

    return  resultMap;
}

void UpdateWorker::getItemInfo(QJsonValue jsonValue, UpdateItemInfo *itemInfo)
{
    if (jsonValue.isNull() || itemInfo == nullptr) {
        return ;
    }

    QStringList language = QLocale::system().name().split('_');
    QString languageType = "CN";
    if (language.count() > 1) {
        languageType = language.value(1);
        if (languageType == "CN"
                || languageType == "TW"
                || languageType == "HK") {
            languageType = "CN";
        } else {
            languageType = "US";
        }
    }

    QJsonObject jsonObject = jsonValue.toObject();

    itemInfo->setPackageId(jsonObject.value("package_id").toString());
    itemInfo->setCurrentVersion(jsonObject.value("current_version_" + languageType).toString());
    itemInfo->setAvailableVersion(jsonObject.value("available_version_" + languageType).toString());
    itemInfo->setExplain(jsonObject.value("update_explain_" + languageType).toString());

    if (jsonObject.contains("update_time_" + languageType)) {
        itemInfo->setUpdateTime(jsonValue.toObject().value("update_time_" + languageType).toString());
    } else {
        itemInfo->setUpdateTime(jsonValue.toObject().value("update_time").toString());
    }

    qDebug() << "UpdateWorker::getItemInfo  itemInfo->name() == " << itemInfo->name();

    QJsonValue dataValue = jsonValue.toObject().value("data_info");
    if (dataValue.isArray()) {
        QJsonArray array = dataValue.toArray();
        QList<DetailInfo> itemList ;
        int count = array.count();
        for (int i = 0; i < count; ++i) {
            DetailInfo detailInfo;
            detailInfo.name = array.at(i).toObject().value("name_" + languageType).toString().trimmed();
            detailInfo.updateTime = array.at(i).toObject().value("update_time").toString().trimmed();
            detailInfo.info = array.at(i).toObject().value("detail_info_" + languageType).toString().trimmed();
            detailInfo.link = array.at(i).toObject().value("link").toString().trimmed();
            if (detailInfo.name.isEmpty()
                    && detailInfo.updateTime.isEmpty()
                    && detailInfo.info.isEmpty()
                    && detailInfo.link.isEmpty()) {
                continue;
            }
            itemList.append(detailInfo);
        }

        if (itemList.count() > 0) {
            itemInfo->setDetailInfos(itemList);
        }
    }
}

bool UpdateWorker::checkDbusIsValid()
{

    if (!checkJobIsValid(m_checkUpdateJob)
            || !checkJobIsValid(m_sysUpdateDownloadJob)
            || !checkJobIsValid(m_sysUpdateInstallJob)
            || !checkJobIsValid(m_safeUpdateDownloadJob)
            || !checkJobIsValid(m_safeUpdateInstallJob)
            || !checkJobIsValid(m_unknownUpdateDownloadJob)
            || !checkJobIsValid(m_unknownUpdateInstallJob)) {

        return false;
    }

    return  true;
}

void UpdateWorker::onSmartMirrorServiceIsValid(bool isvalid)
{
    m_smartMirrorInter->setSync(false);

    if (!isvalid) {
        m_smartMirrorInter->startServiceProcess();
    }
}

//处于以下状态时，就不能再去设置其他更新的状态了，直接显示对应错误提示
bool UpdateWorker::getNotUpdateState()
{
    bool ret = true;
    UpdatesStatus state = m_model->status();

    if (state == UpdatesStatus::NoSpace ||
            state == UpdatesStatus::NoNetwork ||
            state == UpdatesStatus::RecoveryBackupFailed ||
            state == UpdatesStatus::DeependenciesBrokenError ||
            state == UpdatesStatus::UpdateFailed) {
        ret = false;
    }

    return ret;
}

void UpdateWorker::resetDownloadInfo(bool state)
{
    m_downloadSize = 0;
    m_updatableApps.clear();
    m_updatablePackages.clear();

    m_updatePackages.clear();
    m_systemPackages.clear();
    m_safePackages.clear();
    m_unknownPackages.clear();

    if (!state) {
        deleteJob(m_sysUpdateDownloadJob);
        deleteJob(m_sysUpdateInstallJob);
        deleteJob(m_safeUpdateDownloadJob);
        deleteJob(m_safeUpdateInstallJob);
        deleteJob(m_unknownUpdateDownloadJob);
        deleteJob(m_unknownUpdateInstallJob);
        deleteJob(m_checkUpdateJob);
    }
}

CheckUpdateJobRet UpdateWorker::createCheckUpdateJob(const QString &jobPath)
{
    CheckUpdateJobRet ret;
    ret.status = "failed";
    if (m_checkUpdateJob != nullptr) {
        return ret;
    }

    m_checkUpdateJob = new JobInter("com.deepin.lastore", jobPath, QDBusConnection::systemBus(), this);

    ret.jobID = m_checkUpdateJob->id();
    ret.jobDescription = m_checkUpdateJob->description();

    connect(m_checkUpdateJob, &__Job::StatusChanged, this, &UpdateWorker::onCheckUpdateStatusChanged);
    connect(qApp, &QApplication::aboutToQuit, this, [ = ] {
        if (m_checkUpdateJob)
        {
            delete m_checkUpdateJob.data();
        }
    });

    connect(m_checkUpdateJob, &__Job::ProgressChanged, m_model, &UpdateModel::setUpdateProgress, Qt::QueuedConnection);
    m_checkUpdateJob->ProgressChanged(m_checkUpdateJob->progress());
    m_checkUpdateJob->StatusChanged(m_checkUpdateJob->status());

    return  ret;
}

void UpdateWorker::distUpgrade(ClassifyUpdateType updateType)
{
    UpdatesStatus status = m_model->getClassifyUpdateStatus(updateType);

    if (m_backupStatus == BackupStatus::Backingup) {
        QPointer<JobInter> job = getDownloadJob(updateType);
        if (job != nullptr) {
            m_managerInter->CleanJob(job->id());
            deleteJob(job);
        }
        m_model->setClassifyUpdateTypeStatus(updateType, UpdatesStatus::WaitRecoveryBackup);
        return;

    }
    if (m_backupStatus == BackupStatus::Backuped) {
        downloadAndInstallUpdates(updateType);
        return;
    }

    if (hasBackedUp()) {
        m_backupStatus = BackupStatus::Backuped;
        m_backupingClassifyType = ClassifyUpdateType::Invalid;
        downloadAndInstallUpdates(updateType);
        return;
    }

    if (status == UpdatesStatus::Downloading) {
        QPointer<JobInter> job = getDownloadJob(updateType);
        if (job != nullptr) {
            m_managerInter->CleanJob(job->id());
            deleteJob(job);
        }
    }

    m_backupStatus = BackupStatus::Backingup;
    m_backupingClassifyType = updateType;
    //First start backupRecovery , then to load(in RecoveryInter::JobEnd Lemon function)
    bool bConfigVlid = m_model->recoverConfigValid();
    qDebug() << Q_FUNC_INFO << " [abRecovery] 更新前,检查备份配置是否满足(true:满足) : " << bConfigVlid;

    if (bConfigVlid) { //系统环境配置为可以恢复,在收到jobEnd()后,且"backup",成功,后才会继续到下一步下载数据
        recoveryCanBackup(updateType);
    } else { //系统环境配置不满足,则直接跳到下一步下载数据
        qDebug() << Q_FUNC_INFO << " [abRecovery] 备份配置环境不满足,继续更新.";
        m_backupStatus = BackupStatus::Backuped;
        downloadAndInstallUpdates(updateType);
    }

}


void UpdateWorker::setAutoCheckUpdates(const bool autoCheckUpdates)
{
    m_updateInter->SetAutoCheckUpdates(autoCheckUpdates);
}

void UpdateWorker::setUpdateMode(const quint64 updateMode)
{
    qDebug() << Q_FUNC_INFO << "set UpdateMode to dbus:" << updateMode;

    m_managerInter->setUpdateMode(updateMode);
}

void UpdateWorker::setAutoDownloadUpdates(const bool &autoDownload)
{
    m_updateInter->SetAutoDownloadUpdates(autoDownload);
    if (autoDownload == false) {
        m_updateInter->setAutoInstallUpdates(false);
    }
}

void UpdateWorker::setAutoInstallUpdates(const bool &autoInstall)
{
    m_updateInter->setAutoInstallUpdates(autoInstall);
}

void UpdateWorker::setMirrorSource(const MirrorInfo &mirror)
{
    m_updateInter->SetMirrorSource(mirror.m_id);
}

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
void UpdateWorker::setSourceCheck(bool enable)
{
    m_lastoresessionHelper->SetSourceCheckEnabled(enable);
}
#endif

void UpdateWorker::testMirrorSpeed()
{
    QList<MirrorInfo> mirrors = m_model->mirrorInfos();

    QStringList urlList;
    for (MirrorInfo &info : mirrors) {
        urlList << info.m_url;
    }

    // reset the data;
    m_model->setMirrorSpeedInfo(QMap<QString, int>());

    QFutureWatcher<int> *watcher = new QFutureWatcher<int>();
    connect(watcher, &QFutureWatcher<int>::resultReadyAt, [this, urlList, watcher, mirrors](int index) {
        QMap<QString, int> speedInfo = m_model->mirrorSpeedInfo();

        int result = watcher->resultAt(index);
        QString mirrorId = mirrors.at(index).m_id;
        speedInfo[mirrorId] = result;

        m_model->setMirrorSpeedInfo(speedInfo);
    });

    QPointer<QObject> guest(this);
    QFuture<int> future = QtConcurrent::mapped(urlList, std::bind(TestMirrorSpeedInternal, std::placeholders::_1, guest));
    watcher->setFuture(future);
}

void UpdateWorker::checkNetselect()
{
    QProcess *process = new QProcess;
    process->start("netselect", QStringList() << "127.0.0.1");
    connect(process, &QProcess::errorOccurred, this, [this, process](QProcess::ProcessError error) {
        if ((error == QProcess::FailedToStart) || (error == QProcess::Crashed)) {
            m_model->setNetselectExist(false);
            process->deleteLater();
        }
    });
    connect(process, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, [this, process](int result) {
        bool isNetselectExist = 0 == result;
        if (!isNetselectExist) {
            qDebug() << "[wubw UpdateWorker] netselect 127.0.0.1 : " << isNetselectExist;
        }
        m_model->setNetselectExist(isNetselectExist);
        process->deleteLater();
    });
}

void UpdateWorker::setSmartMirror(bool enable)
{
    m_smartMirrorInter->SetEnable(enable);

    QTimer::singleShot(100, this, [ = ] {
        Q_EMIT m_smartMirrorInter->serviceValidChanged(m_smartMirrorInter->isValid());
    });
}

#ifndef DISABLE_SYS_UPDATE_MIRRORS
void UpdateWorker::refreshMirrors()
{
    qDebug() << QDir::currentPath();
    QFile file(":/update/themes/common/config/mirrors.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << file.errorString();
        return;
    }
    QJsonArray array = QJsonDocument::fromJson(file.readAll()).array();
    QList<MirrorInfo> list;
    for (auto item : array) {
        QJsonObject obj = item.toObject();
        MirrorInfo info;
        info.m_id = obj.value("id").toString();
        QString locale = QLocale::system().name();
        if (!(QLocale::system().name() == "zh_CN" || QLocale::system().name() == "zh_TW")) {
            locale = "zh_CN";
        }
        info.m_name = obj.value(QString("name_locale.%1").arg(locale)).toString();
        info.m_url = obj.value("url").toString();
        list << info;
    }
    m_model->setMirrorInfos(list);
    m_model->setDefaultMirror(list[0].m_id);
}
#endif

void UpdateWorker::recoveryCanBackup(ClassifyUpdateType type)
{
    m_model->setStatus(UpdatesStatus::Updateing, __LINE__);
    m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::RecoveryBackingup);
    qDebug() << Q_FUNC_INFO << " [abRecovery] 开始检查是否能备份... ";
    QDBusPendingCall call = m_abRecoveryInter->CanBackup();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, type, call] {
        if (!call.isError())
        {
            QDBusReply<bool> reply = call.reply();
            bool value = reply.value();
            m_model->setRecoverBackingUp(value);
            if (value) {
                qDebug() << Q_FUNC_INFO << " [abRecovery] 可以备份, 开始备份...";

                switch (type) {
                case ClassifyUpdateType::SystemUpdate:
                    setUpdateItemProgress(m_model->systemDownloadInfo(), 0.7);
                    m_model->setSystemUpdateStatus(UpdatesStatus::RecoveryBackingup);
                    break;
                case ClassifyUpdateType::SecurityUpdate:
                    setUpdateItemProgress(m_model->safeDownloadInfo(), 0.7);
                    m_model->setSafeUpdateStatus(UpdatesStatus::RecoveryBackingup);
                    break;
                case ClassifyUpdateType::UnknownUpdate:
                    setUpdateItemProgress(m_model->unknownDownloadInfo(), 0.7);
                    m_model->setUnkonowUpdateStatus(UpdatesStatus::RecoveryBackingup);
                    break;
                default:
                    break;
                }

                m_abRecoveryInter->StartBackup();
            } else {
                m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::RecoveryBackupFailed);
                m_backupStatus = BackupStatus::BackupFailed;
                onRecoveryFinshed(false);
                qDebug() << Q_FUNC_INFO << " [abRecovery] 是否能备份(CanBackup)的环境不满足 -> 备份失败 ";
            }
        } else
        {
            m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::RecoveryBackupFailed);
            m_backupStatus = BackupStatus::BackupFailed;
            onRecoveryFinshed(false);
            qDebug() << " [abRecovery] recovery CanBackup error: " << call.error().message();
        }
    });
}

void UpdateWorker::recoveryStartRestore()
{
    m_abRecoveryInter->StartRestore();
}


void UpdateWorker::setCheckUpdatesJob(const QString &jobPath)
{
    qDebug() << "[setCheckUpdatesJob] start status : " << m_model->status();
    UpdatesStatus state = m_model->status();
    if (UpdatesStatus::Downloading != state && UpdatesStatus::DownloadPaused != state && UpdatesStatus::Installing != state) {
        m_model->setStatus(UpdatesStatus::Checking, __LINE__);
    } else if (UpdatesStatus::UpdateFailed == state) {
        resetDownloadInfo();
    }

    createCheckUpdateJob(jobPath);
}

void UpdateWorker::setDownloadJob(const QString &jobPath, ClassifyUpdateType updateType)
{
    QMutexLocker locker(&m_downloadMutex);
    if (m_model->status() == UpdatesStatus::Default || m_model->status() == UpdatesStatus::Checking) {
        setUpdateInfo();
    }

    m_model->setStatus(UpdatesStatus::Updateing, __LINE__);
    QPointer<JobInter> job = new JobInter("com.deepin.lastore",
                                          jobPath,
                                          QDBusConnection::systemBus(), this);
    switch (updateType) {
    case ClassifyUpdateType::SystemUpdate:
        m_sysUpdateDownloadJob = job;
        connect(m_sysUpdateDownloadJob, &__Job::ProgressChanged, this, &UpdateWorker::onSysUpdateDownloadProgressChanged);
        connect(m_sysUpdateDownloadJob, &__Job::NameChanged, this, &UpdateWorker::setSysUpdateDownloadJobName);
        break;

    case ClassifyUpdateType::SecurityUpdate:
        m_safeUpdateDownloadJob = job;
        connect(m_safeUpdateDownloadJob, &__Job::ProgressChanged, this, &UpdateWorker::onSafeUpdateDownloadProgressChanged);
        connect(m_safeUpdateDownloadJob, &__Job::NameChanged, this, &UpdateWorker::setSafeUpdateDownloadJobName);
        break;

    case ClassifyUpdateType::UnknownUpdate:
        m_unknownUpdateDownloadJob = job;
        connect(m_unknownUpdateDownloadJob, &__Job::ProgressChanged, this, &UpdateWorker::onUnkonwnUpdateDownloadProgressChanged);
        connect(m_unknownUpdateDownloadJob, &__Job::NameChanged, this, &UpdateWorker::setUnknownUpdateDownloadJobName);
        break;

    default:
        break;
    }

    connect(job, &__Job::StatusChanged, this, [ = ](QString status) {
        onClassityDownloadStatusChanged(updateType, status);
    });

    job->StatusChanged(job->status());
    job->ProgressChanged(job->progress());
    job->NameChanged(job->name());
}

void UpdateWorker::setDistUpgradeJob(const QString &jobPath, ClassifyUpdateType updateType)
{
    QMutexLocker locker(&m_mutex);
    m_model->setStatus(UpdatesStatus::Updateing, __LINE__);
    QPointer<JobInter> job = new JobInter("com.deepin.lastore",
                                          jobPath,
                                          QDBusConnection::systemBus(), this);
    switch (updateType) {
    case ClassifyUpdateType::SystemUpdate:
        m_sysUpdateInstallJob = job;
        connect(m_sysUpdateInstallJob, &__Job::ProgressChanged, this, &UpdateWorker::onSysUpdateInstallProgressChanged);
        break;
    case ClassifyUpdateType::SecurityUpdate:
        m_safeUpdateInstallJob = job;
        connect(m_safeUpdateInstallJob, &__Job::ProgressChanged, this, &UpdateWorker::onSafeUpdateInstallProgressChanged);
        break;
    case ClassifyUpdateType::UnknownUpdate:
        m_unknownUpdateInstallJob = job;
        connect(m_unknownUpdateInstallJob, &__Job::ProgressChanged, this, &UpdateWorker::onUnkonwnUpdateInstallProgressChanged);
        break;
    default:
        break;
    }

    connect(job, &__Job::StatusChanged, this, [ = ](QString status) {
        onClassityInstallStatusChanged(updateType, status);
    });

    job->StatusChanged(job->status());
    job->ProgressChanged(job->progress());
}

void UpdateWorker::setReleaseNoteInstallJob(const QString &jobPath)
{
    m_releaseNoteInstallJob = new JobInter("com.deepin.lastore",
                                           jobPath,
                                           QDBusConnection::systemBus(), this);
    connect(m_releaseNoteInstallJob, &__Job::StatusChanged, this, [ = ](const QString & status) {
        qDebug() << "setReleaseNoteInstallJob-----status: " << status;
        setReleaseNoteStatus(status);
        if (status == "failed") {
            m_managerInter->CleanJob(m_releaseNoteInstallJob->id());
            deleteJob(m_releaseNoteInstallJob);
        }

        if (status == "ready") {
            listenReleaseNoteFile();
        }
    });

    m_releaseNoteInstallJob->StatusChanged(m_releaseNoteInstallJob->status());
}

void UpdateWorker::setUpdateItemProgress(UpdateItemInfo *itemInfo, double value)
{
    //异步加载数据,会导致下载信息还未获取就先取到了下载进度
    if (itemInfo) {
        if (!getNotUpdateState()) {
            qDebug() << " Now can't to update continue...";
            resetDownloadInfo();
            return;
        }
        itemInfo->setDownloadProgress(value);

    } else {
        //等待下载信息加载后,再通过 onNotifyDownloadInfoChanged() 设置"UpdatesStatus::Downloading"状态
        qDebug() << "[wubw download] DownloadInfo is nullptr , waitfor download info";
    }
}

bool UpdateWorker::hasBackedUp()
{
    return m_abRecoveryInter->hasBackedUp();
}

void UpdateWorker::onRecoveryFinshed(bool successed)
{

    auto requestUpdate = [ = ](ClassifyUpdateType type)->bool{
        if (m_model->getClassifyUpdateStatus(type) == UpdatesStatus::WaitRecoveryBackup
                || m_model->getClassifyUpdateStatus(type) == UpdatesStatus::RecoveryBackingup
                || m_model->getClassifyUpdateStatus(type) == UpdatesStatus::RecoveryBackingSuccessed)
        {
            distUpgrade(type);
            return true;
        }
        return  false;
    };
    if (successed) {
        requestUpdate(ClassifyUpdateType::SystemUpdate);
        requestUpdate(ClassifyUpdateType::SecurityUpdate);
        requestUpdate(ClassifyUpdateType::UnknownUpdate);
    } else {
        if (requestUpdate(ClassifyUpdateType::SystemUpdate)
                ||  requestUpdate(ClassifyUpdateType::SecurityUpdate)
                ||  requestUpdate(ClassifyUpdateType::UnknownUpdate)) {
            return;
        }
    }
}

void UpdateWorker::setAutoCleanCache(const bool autoCleanCache)
{
    m_managerInter->SetAutoClean(autoCleanCache);
}

void UpdateWorker::onJobListChanged(const QList<QDBusObjectPath> &jobs)
{
    for (const auto &job : jobs) {
        m_jobPath = job.path();

        JobInter jobInter("com.deepin.lastore", m_jobPath, QDBusConnection::systemBus());

        // id maybe scrapped
        const QString &id = jobInter.id();
        // 防止刚打开控制中心的时候获取joblist的时候job还存在，由于构建jobInter可能会花销一定时间导致构建完成后job已经完成，这个时候需要设置对应的更新状态为更新成功
        if (id.isEmpty() && !m_jobPath.isEmpty()) {
            if (m_jobPath.contains("system_upgrade")) {
                m_model->setClassifyUpdateTypeStatus(ClassifyUpdateType::SystemUpdate, UpdatesStatus::UpdateSucceeded);
            } else if (m_jobPath.contains("security_upgrade")) {
                m_model->setClassifyUpdateTypeStatus(ClassifyUpdateType::SecurityUpdate, UpdatesStatus::UpdateSucceeded);
            } else if (m_jobPath.contains("unknown_upgrade")) {
                m_model->setClassifyUpdateTypeStatus(ClassifyUpdateType::UnknownUpdate, UpdatesStatus::UpdateSucceeded);
            }
            continue;
        }

        if (!jobInter.isValid())
            continue;

        qDebug() << "[wubw] onJobListChanged, id : " << id << " , m_jobPath : " << m_jobPath;
        if ((id == "update_source" || id == "custom_update") && m_checkUpdateJob == nullptr) {
            setCheckUpdatesJob(m_jobPath);
        } else if (id == "prepare_system_upgrade" && m_sysUpdateDownloadJob == nullptr) {
            setDownloadJob(m_jobPath, ClassifyUpdateType::SystemUpdate);
        } else if (id == "prepare_security_upgrade" && m_safeUpdateDownloadJob == nullptr) {
            setDownloadJob(m_jobPath, ClassifyUpdateType::SecurityUpdate);
        } else if (id == "prepare_unknown_upgrade" && m_unknownUpdateDownloadJob == nullptr) {
            setDownloadJob(m_jobPath, ClassifyUpdateType::UnknownUpdate);
        } else if (id == "system_upgrade" && m_sysUpdateInstallJob == nullptr) {
            setDistUpgradeJob(m_jobPath, ClassifyUpdateType::SystemUpdate);
        } else if (id == "security_upgrade" && m_safeUpdateInstallJob == nullptr) {
            setDistUpgradeJob(m_jobPath, ClassifyUpdateType::SecurityUpdate);
        } else if (id == "unknown_upgrade" && m_unknownUpdateInstallJob == nullptr) {
            setDistUpgradeJob(m_jobPath, ClassifyUpdateType::UnknownUpdate);
        } else if (id.contains("install") && jobInter.packages().at(0) == "uos-release-note") {
            qDebug() << "jobInter :uos-release-note status: " << jobInter.status();
            qDebug() << "jobInter :uos-release-note packages: " << jobInter.packages();
            setReleaseNoteInstallJob(m_jobPath);
        }
    }
}

void UpdateWorker::onSysUpdateDownloadProgressChanged(double value)
{
    UpdateItemInfo *itemInfo = m_model->systemDownloadInfo();
    setUpdateItemProgress(itemInfo, value);
}

void UpdateWorker::onSafeUpdateDownloadProgressChanged(double value)
{
    UpdateItemInfo *itemInfo = m_model->safeDownloadInfo();

    setUpdateItemProgress(itemInfo, value);
}

void UpdateWorker::onUnkonwnUpdateDownloadProgressChanged(double value)
{
    UpdateItemInfo *itemInfo = m_model->unknownDownloadInfo();

    setUpdateItemProgress(itemInfo, value);

}

void UpdateWorker::onSysUpdateInstallProgressChanged(double value)
{
    UpdateItemInfo *itemInfo = m_model->systemDownloadInfo();
    if (itemInfo == nullptr || qFuzzyIsNull(value)) {
        return;
    }

    setUpdateItemProgress(itemInfo, value);

}

void UpdateWorker::onSafeUpdateInstallProgressChanged(double value)
{
    UpdateItemInfo *itemInfo = m_model->safeDownloadInfo();
    if (itemInfo == nullptr || qFuzzyIsNull(value)) {
        return;
    }

    setUpdateItemProgress(itemInfo, value);
}

void UpdateWorker::onUnkonwnUpdateInstallProgressChanged(double value)
{
    UpdateItemInfo *itemInfo = m_model->unknownDownloadInfo();
    if (itemInfo == nullptr || qFuzzyIsNull(value)) {
        return;
    }

    qDebug() << "onUnkonwnUpdateInstallProgressChanged : " << value;
    setUpdateItemProgress(itemInfo, value);
}

void UpdateWorker::onIconThemeChanged(const QString &theme)
{
    m_iconThemeState = theme;
}

void UpdateWorker::onCheckUpdateStatusChanged(const QString &value)
{
    qDebug() << "[setCheckUpdatesJob]status is: " << value;
    if (value == "failed" || value.isEmpty()) {
        qWarning() << "check for updates job failed";
        if (m_checkUpdateJob != nullptr) {
            m_managerInter->CleanJob(m_checkUpdateJob->id());
            checkDiskSpace(m_checkUpdateJob->description());
            deleteJob(m_checkUpdateJob);
        }
    } else if (value == "success" || value == "succeed") {
        setUpdateInfo();
    } else if (value == "end") {
        deleteJob(m_checkUpdateJob);
    }
}

void UpdateWorker::checkDiskSpace(const QString &jobDescription)
{
    UpdateJobErrorMessage errorMessage = analyzeJobErrorMessage(jobDescription);
    qDebug() << "job description: " << jobDescription;

    if (errorMessage.jobErrorType == "unmetDependencies" ||
            !m_lastoresessionHelper->IsDiskSpaceSufficient()) {
        m_model->setStatus(UpdatesStatus::NoSpace, __LINE__);
    } else if (errorMessage.jobErrorType == "fetchFailed") {
        m_model->setStatus(UpdatesStatus::NoNetwork, __LINE__);
    } else if (errorMessage.jobErrorType == "insufficientSpace") {
        m_model->setStatus(UpdatesStatus::DeependenciesBrokenError, __LINE__);
    } else {
        m_model->setStatus(UpdatesStatus::UpdateFailed, __LINE__);
        qDebug() << Q_FUNC_INFO << "UpdateFailed , jobDescription : " << jobDescription;
    }
    //以上错误均需重置更新信息
    if (m_model->status() == UpdatesStatus::UpdateFailed) {
        resetDownloadInfo();
    }
}

void UpdateWorker::setBatteryPercentage(const BatteryPercentageInfo &info)
{
    m_batteryPercentage = info.value("Display", 0);
    const bool low = m_onBattery && m_batteryPercentage < 50;
    m_model->setLowBattery(low);
}

//Now D-Bus only in system power have BatteryPercentage data
void UpdateWorker::setSystemBatteryPercentage(const double &value)
{
    m_batterySystemPercentage = value;
    const bool low = m_onBattery && m_batterySystemPercentage < 50;
    m_model->setLowBattery(low);
}

void UpdateWorker::setOnBattery(bool onBattery)
{
    m_onBattery = onBattery;
    const bool low = m_onBattery && m_batteryPercentage < 50;
    // const bool low = m_onBattery ? m_batterySystemPercentage < 50 : false;
    m_model->setLowBattery(low);
}

void UpdateWorker::refreshHistoryAppsInfo()
{
    //m_model->setHistoryAppInfos(m_updateInter->getHistoryAppsInfo());
    m_model->setHistoryAppInfos(m_updateInter->ApplicationUpdateInfos(QLocale::system().name()));
}

void UpdateWorker::refreshLastTimeAndCheckCircle()
{
    QString checkTime;
    double interval = m_updateInter->GetCheckIntervalAndTime(checkTime);

    m_model->setAutoCheckUpdateCircle(static_cast<int>(interval));
    m_model->setLastCheckUpdateTime(checkTime);
}

void UpdateWorker::setUpdateNotify(const bool notify)
{
    m_updateInter->SetUpdateNotify(notify);
}

void UpdateWorker::OnDownloadJobCtrl(ClassifyUpdateType type, int updateCtrlType)
{
    QPointer<JobInter> job = getDownloadJob(type);

    if (job == nullptr) {
        return;
    }

    switch (updateCtrlType) {
    case UpdateCtrlType::Start:
        m_managerInter->StartJob(job->id());
        break;
    case UpdateCtrlType::Pause:
        m_managerInter->PauseJob(job->id());
        break;
    }
}

void UpdateWorker::downloadAndInstallUpdates(ClassifyUpdateType updateType)
{
    uint64_t type = (uint64_t)updateType;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_managerInter->ClassifiedUpgrade(type), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher, updateType] {
        if (!watcher->isError())
        {
            watcher->reply().path();
            QDBusPendingReply<QList<QDBusObjectPath> > reply = watcher->reply();
            QList<QDBusObjectPath>  data = reply.value();
            if (data.count() < 1) {
                qDebug() << "UpdateFailed, download updates error: " << watcher->error().message();
                return;
            }
            setDownloadJob(reply.value().at(0).path(), updateType);
        } else
        {
            m_model->setClassifyUpdateTypeStatus(updateType, UpdatesStatus::UpdateFailed);
            resetDownloadInfo();
            QPointer<JobInter>  job = getDownloadJob(updateType);
            if (!job.isNull()) {
                m_managerInter->CleanJob(job->id());
            }

            job = getInstallJob(updateType);
            if (!job.isNull()) {
                m_managerInter->CleanJob(job->id());
            }
            qDebug() << "UpdateFailed, download updates error: " << watcher->error().message();
        }
    });
}

void UpdateWorker::onRecoveryBackupFinshed(const QString &kind, const bool success, const QString &errMsg)
{
    qDebug() << " [abRecovery] RecoveryInter::JobEnd 备份结果 -> kind : " << kind << " , success : " << success << " , errMsg : " << errMsg;
    //kind 在备份时为 "backup"，在恢复时为 "restore" (此处为备份)
    if ("backup" == kind) {
        //失败:提示失败,不再进行更新进行
        if (!success) {
            m_model->setClassifyUpdateTypeStatus(m_backupingClassifyType, UpdatesStatus::RecoveryBackupFailed);
            qDebug() << Q_FUNC_INFO << " [abRecovery] 备份失败 , errMsg : " << errMsg;
            m_backupStatus = BackupStatus::BackupFailed;
            onRecoveryFinshed(false);
            return;
        }

        m_backupStatus = BackupStatus::Backuped;
        m_model->setClassifyUpdateTypeStatus(m_backupingClassifyType, UpdatesStatus::RecoveryBackingSuccessed);
        onRecoveryFinshed(true);
    }
}

QPointer<JobInter> UpdateWorker::getDownloadJob(ClassifyUpdateType updateType)
{
    QPointer<JobInter> job;
    switch (updateType) {
    case ClassifyUpdateType::SystemUpdate:
        job = m_sysUpdateDownloadJob;
        break;
    case ClassifyUpdateType::SecurityUpdate:
        job = m_safeUpdateDownloadJob;
        break;
    case ClassifyUpdateType::UnknownUpdate:
        job = m_unknownUpdateDownloadJob;
        break;
    default:
        job = nullptr;
        break;
    }

    return job;
}

QPointer<JobInter> UpdateWorker::getInstallJob(ClassifyUpdateType updateType)
{
    QPointer<JobInter> job;
    switch (updateType) {
    case ClassifyUpdateType::SystemUpdate:
        job = m_sysUpdateInstallJob;
        break;
    case ClassifyUpdateType::SecurityUpdate:
        job = m_safeUpdateInstallJob;
        break;
    case ClassifyUpdateType::UnknownUpdate:
        job = m_unknownUpdateInstallJob;
        break;
    default:
        job = nullptr;
        break;
    }

    return job;
}

bool UpdateWorker::checkJobIsValid(QPointer<JobInter> dbusJob)
{
    if (!dbusJob.isNull()) {
        if (dbusJob->isValid() && getNotUpdateState()) {
            return true;
        } else {
            dbusJob->deleteLater();
            return  false;
        }
    }

    return  false;
}

void UpdateWorker::deleteJob(QPointer<JobInter> dbusJob)
{
    if (!dbusJob.isNull()) {
        dbusJob->deleteLater();
        dbusJob = nullptr;
    }
}

void UpdateWorker::deleteClassityDownloadJob(ClassifyUpdateType type)
{
    switch (type) {
    case ClassifyUpdateType::SystemUpdate:
        deleteJob(m_sysUpdateDownloadJob);
        break;
    case ClassifyUpdateType::SecurityUpdate:
        deleteJob(m_safeUpdateDownloadJob);
        break;
    case ClassifyUpdateType::UnknownUpdate:
        deleteJob(m_unknownUpdateDownloadJob);
        break;
    default:
        break;
    }
}

void UpdateWorker::deleteClassityInstallJob(ClassifyUpdateType type)
{
    switch (type) {
    case ClassifyUpdateType::SystemUpdate:
        deleteJob(m_sysUpdateInstallJob);
        break;
    case ClassifyUpdateType::SecurityUpdate:
        deleteJob(m_safeUpdateInstallJob);
        break;
    case ClassifyUpdateType::UnknownUpdate:
        deleteJob(m_unknownUpdateInstallJob);
        break;
    default:
        break;
    }
}

bool UpdateWorker::checkUpdateSuccessed()
{
    if ((m_model->getSystemUpdateStatus() == UpdatesStatus::UpdateSucceeded || m_model->getSystemUpdateStatus() == UpdatesStatus::Default)
            && (m_model->getSafeUpdateStatus() == UpdatesStatus::UpdateSucceeded || m_model->getSafeUpdateStatus() == UpdatesStatus::Default)
            && (m_model->getUnkonowUpdateStatus() == UpdatesStatus::UpdateSucceeded || m_model->getUnkonowUpdateStatus() == UpdatesStatus::Default)) {
        QFile file("/tmp/.dcc-update-successd");
        if (file.exists())
            return true;
        file.open(QIODevice::WriteOnly);
        file.close();
        return  true;
    }

    return  false;
}

void UpdateWorker::cleanLastoreJob(QPointer<JobInter> dbusJob)
{
    if (dbusJob != nullptr) {
        m_managerInter->CleanJob(dbusJob->id());
        deleteJob(dbusJob);
    }
}

void UpdateWorker::setUnknownUpdateDownloadJobName(const QString &unknownUpdateDownloadJobName)
{
    m_unknownUpdateDownloadJobName = unknownUpdateDownloadJobName;
}


void UpdateWorker::setSafeUpdateDownloadJobName(const QString &safeUpdateDownloadJobName)
{
    m_safeUpdateDownloadJobName = safeUpdateDownloadJobName;
}

void UpdateWorker::setSysUpdateDownloadJobName(const QString &sysUpdateDownloadJobName)
{
    m_sysUpdateDownloadJobName = sysUpdateDownloadJobName;
}

void UpdateWorker::onRequestOpenAppStore()
{
    QDBusInterface appStore("com.home.appstore.client",
                            "/com/home/appstore/client",
                            "com.home.appstore.client",
                            QDBusConnection::sessionBus());
    QVariant value = "tab/update";
    QDBusMessage reply = appStore.call("openBusinessUri", value);
    qDebug() << reply.errorMessage();
}

UpdateJobErrorMessage UpdateWorker::analyzeJobErrorMessage(QString jobDescription)
{
    UpdateJobErrorMessage result;
    result.jobErrorType = "InvalidJobType";
    result.jobErrorMessage = "";
    QJsonParseError err_rpt;
    QJsonDocument jobErrorMessage = QJsonDocument::fromJson(jobDescription.toUtf8(), &err_rpt);

    if (err_rpt.error != QJsonParseError::NoError) {
        qDebug() << "更新日志信息JSON格式错误";
        return result;
    }
    const QJsonObject &object = jobErrorMessage.object();
    QString errorType =  object.value("ErrType").toString();
    if (errorType.contains("fetchFailed", Qt::CaseInsensitive) || errorType.contains("IndexDownloadFailed", Qt::CaseInsensitive)) {
        result.jobErrorType = "fetchFailed";
        result.jobErrorMessage = tr("Network error");
    } else if (errorType.contains("unmetDependencies", Qt::CaseInsensitive) || errorType.contains("dependenciesBroken", Qt::CaseInsensitive)) {
        result.jobErrorType = "unmetDependencies";
        result.jobErrorMessage = tr("Dependency error");
    } else if (errorType.contains("insufficientSpace", Qt::CaseInsensitive)) {
        result.jobErrorType = "insufficientSpace";
        result.jobErrorMessage = tr("Insufficient disk space");
    } else {
        result.jobErrorType = "unknown";
        result.jobErrorMessage = tr("Update failed");
    }

    return result;
}

void UpdateWorker::onClassityDownloadStatusChanged(const ClassifyUpdateType type, const QString &value)
{
    qDebug() << "onClassityDownloadStatusChanged ::" << type << "status :: " << value;
    if (value == "running" || value == "ready") {
        m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::Downloading);
    } else if (value == "failed") {
        QPointer<JobInter> job = getDownloadJob(type);
        qDebug() << "onClassityDownloadStatusChanged ::" << type << "job->description() :: " << job->description();
        m_model->setClassityUpdateJonError(type, analyzeJobErrorMessage(job->description()));
        m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::UpdateFailed);
        cleanLastoreJob(job);
    } else if (value == "succeed") {
        if (getClassityUpdateDownloadJobName(type).contains("OnlyDownload")) {
            m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::AutoDownloaded);
        } else {
            m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::Downloaded);
        }
    } else if (value == "paused") {
        m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::DownloadPaused);
    } else if (value == "end") {
        deleteClassityDownloadJob(type);
    }
}

void UpdateWorker::onClassityInstallStatusChanged(const ClassifyUpdateType type, const QString &value)
{
    qDebug() << "onClassityInstallStatusChanged ::" << type << "status :: " << value;
    if (value == "ready") {
        m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::Downloaded);
    } else if (value == "running") {
        m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::Installing);
    } else if (value == "failed") {
        QPointer<JobInter> job = getInstallJob(type);
        qDebug() << "onClassityInstallStatusChanged ::" << type << "job->description() :: " << job->description();
        m_model->setClassityUpdateJonError(type, analyzeJobErrorMessage(job->description()));
        m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::UpdateFailed);
        cleanLastoreJob(job);
    } else if (value == "succeed") {
        m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::UpdateSucceeded);
    } else if (value == "end") {
        if (checkUpdateSuccessed()) {
            m_model->setStatus(UpdatesStatus::UpdateSucceeded);
        }
        deleteClassityInstallJob(type);
    }
}

QString UpdateWorker::getClassityUpdateDownloadJobName(ClassifyUpdateType updateType)
{
    QString value = "";
    switch (updateType) {
    case ClassifyUpdateType::SystemUpdate:
        value = m_sysUpdateDownloadJobName;
        break;
    case ClassifyUpdateType::SecurityUpdate:
        value = m_safeUpdateDownloadJobName;
        break;
    case ClassifyUpdateType::UnknownUpdate:
        value = m_unknownUpdateDownloadJobName;
        break;
    default:
        break;
    }
    return  value;
}

void UpdateWorker::listenReleaseNoteFile()
{
	if(m_fileSystemWatcher == nullptr){
		m_fileSystemWatcher = new QFileSystemWatcher;
	}
    if (QFile::exists(ChangeLogFile)) {
        m_fileSystemWatcher->addPath(ChangeLogFile);
        connect(m_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, [ = ](const QString & path) {
            qDebug() << "fileChanged    --" << path;
            m_releaseNoteUpdated = true;
            setReleaseNoteStatus(m_releaseNoteJobStatus);
        });
        return;
    }

    QDir dir(ChangeLogDic + "release-note");
    QString listenDir = dir.exists() ? (ChangeLogDic + "release-note") : ChangeLogDic;
    m_fileSystemWatcher->addPath(listenDir);
    connect(m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, [ = ](const QString & path) {
        qDebug() << "directoryChangedpath :" << path;
        QDir dir(ChangeLogDic + "release-note");
        if (dir.exists()) {
            if (m_fileSystemWatcher->directories().contains(ChangeLogDic)) {
                m_fileSystemWatcher->removePath(ChangeLogDic);
            }
            if (!m_fileSystemWatcher->directories().contains(ChangeLogDic + "release-note")) {
                m_fileSystemWatcher->addPath(ChangeLogDic + "release-note");
            }
            if (QFile::exists(ChangeLogFile)) {
                m_releaseNoteUpdated = true;
                setReleaseNoteStatus(m_releaseNoteJobStatus);
            }
        }
    });

}

QString UpdateWorker::getReleaseNoteStatus() const
{
    return m_releaseNoteJobStatus;
}

void UpdateWorker::setReleaseNoteStatus(const QString &releaseNoteStatus)
{
    m_releaseNoteJobStatus = releaseNoteStatus;

    if (m_releaseNoteJobStatus == "failed") {
        Q_EMIT releaseNoteInstallCompleted();
    }

    if (m_releaseNoteUpdated && m_releaseNoteJobStatus == "end") {
        Q_EMIT releaseNoteInstallCompleted();
    }
}

void UpdateWorker::onClassifiedUpdatablePackagesChanged(QMap<QString, QStringList> packages)
{
    m_systemPackages = packages.value(SystemUpdateType);
    if (m_systemPackages.count() == 0) {
        m_model->setClassifyUpdateTypeStatus(ClassifyUpdateType::SystemUpdate, UpdatesStatus::Default);
    }
    m_safePackages = packages.value(SecurityUpdateType);
    if (m_safePackages.count() == 0) {
        m_model->setClassifyUpdateTypeStatus(ClassifyUpdateType::SecurityUpdate, UpdatesStatus::Default);
    }
    m_unknownPackages = packages.value(UnknownUpdateType);
    if (m_unknownPackages.count() == 0) {
        m_model->setClassifyUpdateTypeStatus(ClassifyUpdateType::UnknownUpdate, UpdatesStatus::Default);
    }
}

void UpdateWorker::setUpdateItemDownloadSize(UpdateItemInfo *updateItem,  QStringList packages)
{
    QDBusPendingCall call = m_managerInter->PackagesDownloadSize(packages);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, updateItem, call] {
        if (!call.isError())
        {
            QDBusReply<qlonglong> reply = call.reply();
            qlonglong value = reply.value();
            updateItem->setDownloadSize(value);
        }
    });
}

}
}
