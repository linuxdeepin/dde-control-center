// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "updatework.h"

#include "common.h"
#include "mirrorinfolist.h"

#include <DConfig>
#include <DDialog>
#include <DLabel>
#include <DNotifySender>
#include <DWaterProgress>

#include <QApplication>
#include <QDesktopServices>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLoggingCategory>
#include <QMutexLocker>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <QtConcurrent>

#include <mutex>

Q_LOGGING_CATEGORY(DccUpdateWork, "dcc-update-work")

#define MIN_NM_ACTIVE 50
#define UPDATE_PACKAGE_SIZE 0

const QString ChangeLogFile = "/usr/share/deepin/release-note/UpdateInfo.json";
const QString ChangeLogDic = "/usr/share/deepin/";
const QString UpdateLogTmpFile = "/tmp/deepin-update-log.json";

constexpr int LogTypeSystem = 1;               // 系统更新
constexpr int LogTypeSecurity = 2;             // 安全更新
constexpr int DesktopProfessionalPlatform = 1; // 桌面专业版
constexpr int DesktopCommunityPlatform = 3;    // 桌面社区版
constexpr int ServerPlatform = 6;              // 服务器版

constexpr int RecoveryBackupFailedDistFull = -2; // Not enough disk space

#define CHECK_JOBS_ENV "DCC_PACKAGE_CHECK_JOBS"

// NOTE: start with ii, any space, anychar, any space, anychar, at least one space, anykind of char
static const QString PACKAGE_REGEX = QStringLiteral("^ii [ \t]+([^ ]+)[ \t]+([^ ]+)[ \t]+.*$");

using Dtk::Widget::DDialog;
using Dtk::Widget::DLabel;
using Dtk::Widget::DWaterProgress;

std::mutex CHECK_CANEXIST_GUARD;

inline void notifyError(const QString &summary, const QString &body)
{
    DUtil::DNotifySender(summary)
            .appIcon("dde-control-center")
            .appName("dde-control-center")
            .appBody(body)
            .timeOut(5000)
            .call();
}

inline void notifyErrorWithoutBody(const QString &summary)
{
    DUtil::DNotifySender(summary)
            .appIcon("dde-control-center")
            .appName("dde-control-center")
            .timeOut(5000)
            .call();
}

static int getPlatform()
{
    if (IsServerSystem) {
        return ServerPlatform;
    }

    if (IsCommunitySystem) {
        return DesktopCommunityPlatform;
    }

    return DesktopProfessionalPlatform;
}


static int TestMirrorSpeedInternal(const QString& url, QPointer<QObject> baseObject)
{
    if (!baseObject || QCoreApplication::closingDown()) {
        return -1;
    }

    QStringList args;
    args << url << "-s"
         << "1";

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

    if (!result.first().isEmpty()) {
        return result.first().toInt();
    }

    return 10000;
}

UpdateWorker::UpdateWorker(UpdateModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_checkUpdateJob(nullptr)
    , m_fixErrorJob(nullptr)
    , m_sysUpdateDownloadJob(nullptr)
    , m_safeUpdateDownloadJob(nullptr)
    , m_unknownUpdateDownloadJob(nullptr)
    , m_sysUpdateInstallJob(nullptr)
    , m_safeUpdateInstallJob(nullptr)
    , m_unknownUpdateInstallJob(nullptr)
    , m_updateInter(new UpdateDBusProxy(this))
    , m_onBattery(true)
    , m_batteryPercentage(0.0)
    , m_batterySystemPercentage(0.0)
    , m_jobPath("")
    , m_downloadSize(0)
    , m_backupStatus(BackupStatus::NoBackup)
    , m_backupingClassifyType(ClassifyUpdateType::Invalid)
    , m_machineid(std::nullopt)
    , m_testingChannelUrl(std::nullopt)
    , m_isFirstActive(true)
    , m_updateConfig(DConfig::create("org.deepin.dde.control-center", QStringLiteral("org.deepin.dde.control-center.update"), QString(), this))
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
    deleteJob(m_fixErrorJob);
}

void UpdateWorker::preInitialize()
{
    // 是否开启更新提示
    connect(m_updateInter,
            &UpdateDBusProxy::UpdateNotifyChanged,
            m_model,
            &UpdateModel::setUpdateNotify);
    m_model->setUpdateMode(m_updateInter->updateMode());
    m_model->setUpdateNotify(m_updateInter->updateNotify());

    QFutureWatcher<QMap<QString, QStringList>> *packagesWatcher =
            new QFutureWatcher<QMap<QString, QStringList>>(this);
    connect(packagesWatcher, &QFutureWatcher<QStringList>::finished, this, [=] {
        QMap<QString, QStringList> updatablePackages = packagesWatcher->result();
        checkUpdatablePackages(updatablePackages);
        packagesWatcher->deleteLater();
    });

    packagesWatcher->setFuture(QtConcurrent::run([=]() -> QMap<QString, QStringList> {
        QMap<QString, QStringList> map;
        map = m_updateInter->classifiedUpdatablePackages();
        return map;
    }));
}

void UpdateWorker::init()
{
    qRegisterMetaType<UpdatesStatus>("UpdatesStatus");
    qRegisterMetaType<UiActiveState>("UiActiveState");
    qRegisterMetaType<TestingChannelStatus>("TestingChannelStatus");

    QString sVersion =
            QString("%1 %2").arg(DSysInfo::uosProductTypeName()).arg(DSysInfo::majorVersion());
    if (!IsServerSystem)
        sVersion.append(" " + DSysInfo::uosEditionName());
    m_model->setSystemVersionInfo(sVersion);

    // clang-format off
    connect(m_updateInter, &UpdateDBusProxy::JobListChanged, this, &UpdateWorker::onJobListChanged);
    connect(m_updateInter, &UpdateDBusProxy::AutoCleanChanged,
            m_model, &UpdateModel::setAutoCleanCache);

    connect(m_updateInter, &UpdateDBusProxy::AutoDownloadUpdatesChanged,
            m_model, &UpdateModel::setAutoDownloadUpdates);
    connect(m_updateInter, &UpdateDBusProxy::AutoInstallUpdatesChanged,
            m_model, &UpdateModel::setAutoInstallUpdates);
    connect(m_updateInter, &UpdateDBusProxy::AutoInstallUpdateTypeChanged,
            m_model, &UpdateModel::setAutoInstallUpdateType);
    connect(m_updateInter, &UpdateDBusProxy::AutoCheckUpdatesChanged,
            m_model, &UpdateModel::setAutoCheckUpdates);
    connect(m_updateConfig, &DConfig::valueChanged, m_model, [=](const QString &key){
        if (key == "backup") {
            m_model->setBackupUpdates(m_updateConfig->value("backup", true).toBool());
        }
    });
    connect(m_updateInter, &UpdateDBusProxy::UpdateModeChanged, m_model, [=](qulonglong value) {
        m_model->setUpdateMode(value);
        QMap<QString, QStringList> updatablePackages = m_updateInter->classifiedUpdatablePackages();
        checkUpdatablePackages(updatablePackages);
    });

    connect(m_updateInter, &UpdateDBusProxy::ClassifiedUpdatablePackagesChanged,
            this, &UpdateWorker::onClassifiedUpdatablePackagesChanged);
    connect(m_updateInter, &UpdateDBusProxy::OnBatteryChanged, this, &UpdateWorker::setOnBattery);
    connect(m_updateInter, &UpdateDBusProxy::BatteryPercentageChanged,
            this, &UpdateWorker::setBatteryPercentage);
    connect(m_updateInter, &UpdateDBusProxy::StateChanged,
            this, &UpdateWorker::handleAtomicStateChanged);
    // clang-format on
    if (IsCommunitySystem) {
        refreshMirrors();
        m_model->setSmartMirrorSwitch(m_updateInter->enable());
        connect(m_updateInter, &UpdateDBusProxy::EnableChanged, m_model, &UpdateModel::setSmartMirrorSwitch);
        connect(m_updateInter, &UpdateDBusProxy::MirrorSourceChanged, m_model, &UpdateModel::setDefaultMirror);
    }
}

void UpdateWorker::licenseStateChangeSlot()
{
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
    connect(watcher, &QFutureWatcher<void>::finished, watcher, &QFutureWatcher<void>::deleteLater);

    QFuture<void> future = QtConcurrent::run(this, &UpdateWorker::getLicenseState);
    watcher->setFuture(future);
}

void UpdateWorker::testingChannelChangeSlot()
{
    if (!IsCommunitySystem) {
        m_model->setTestingChannelStatus(TestingChannelStatus::DeActive);
        return;
    }
    QDBusPendingCall call = m_updateInter->PackageExists(TestingChannelPackage);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [watcher, call, this] {
        if (!call.isError()) {
            QDBusPendingReply<bool> reply = call.reply();
            if (reply.value()) {
                m_model->setTestingChannelStatus(TestingChannelStatus::Joined);
            };
        }
        watcher->deleteLater();
    });
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
        qCDebug(DccUpdateWork) << "com.deepin.license error ," << licenseInfo.lastError().name();
        return;
    }
    UiActiveState reply =
            static_cast<UiActiveState>(licenseInfo.property("AuthorizationState").toInt());
    qCDebug(DccUpdateWork) << "Authorization State:" << reply;
    m_model->setSystemActivation(reply);
}

void UpdateWorker::activate()
{
    if (m_isFirstActive) {
        init();
        preInitialize();
        m_isFirstActive = false;
    }
    QString checkTime;
    double interval = m_updateInter->GetCheckIntervalAndTime(checkTime);
    m_model->setLastCheckUpdateTime(checkTime);
    m_model->setAutoCheckUpdateCircle(static_cast<int>(interval));

    m_model->setAutoCleanCache(m_updateInter->autoClean());
    m_model->setAutoDownloadUpdates(m_updateInter->autoDownloadUpdates());
    m_model->setAutoInstallUpdates(m_updateInter->autoInstallUpdates());
    m_model->setAutoInstallUpdateType(m_updateInter->autoInstallUpdateType());
    m_model->setBackupUpdates(m_updateConfig->value("backup", true).toBool());
    m_model->setAutoCheckUpdates(m_updateInter->autoCheckUpdates());
    m_model->setUpdateMode(m_updateInter->updateMode());
    m_model->setUpdateNotify(m_updateInter->updateNotify());

    setOnBattery(m_updateInter->onBattery());
    setBatteryPercentage(m_updateInter->batteryPercentage());

    const QList<QDBusObjectPath> jobs = m_updateInter->jobList();
    if (jobs.count() > 0) {
        for (QDBusObjectPath dBusObjectPath : jobs) {
            if (dBusObjectPath.path().contains("upgrade")) {
                qCDebug(DccUpdateWork)
                        << "UpdateWorker::activate, jobs.count() == " << jobs.count();
                setUpdateInfo();
                break;
            }
        }
    }

    onJobListChanged(m_updateInter->jobList());

    testingChannelChangeSlot();

    licenseStateChangeSlot();

    QDBusConnection::systemBus().connect("com.deepin.license",
                                         "/com/deepin/license/Info",
                                         "com.deepin.license.Info",
                                         "LicenseStateChange",
                                         this,
                                         SLOT(licenseStateChangeSlot()));
}

void UpdateWorker::deactivate() { }

void UpdateWorker::checkForUpdates()
{
    setOnBattery(m_updateInter->onBattery());
    if (checkDbusIsValid()) {
        qCDebug(DccUpdateWork) << " checkDbusIsValid . do nothing";
        return;
    }

    QDBusPendingCall call = m_updateInter->UpdateSource();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] {
        if (!call.isError()) {
            QDBusReply<QDBusObjectPath> reply = call.reply();
            const QString jobPath = reply.value().path();
            setCheckUpdatesJob(jobPath);
        } else {
            m_model->setStatus(UpdatesStatus::UpdateFailed, __LINE__);
            resetDownloadInfo();
            if (!m_checkUpdateJob.isNull()) {
                m_updateInter->CleanJob(m_checkUpdateJob->id());
            }
            qCDebug(DccUpdateWork)
                    << "UpdateFailed, check for updates error: " << call.error().message();
        }
    });
    // 每次检查更新的时候都从服务器请求一次更新日志
    requestUpdateLog();
}

void UpdateWorker::requestUpdateLog()
{
    qInfo() << "Get update info";
    // 接收并处理respond
    QNetworkAccessManager *http = new QNetworkAccessManager(this);
    connect(http, &QNetworkAccessManager::finished, this, [this, http](QNetworkReply *reply) {
        handleUpdateLogsReply(reply);
        reply->deleteLater();
        http->deleteLater();
    });

    // 请求头
    QNetworkRequest request;
    QUrl url(getUpdateLogAddress());
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("platformType", QByteArray::number(getPlatform()));
    urlQuery.addQueryItem("isUnstable", QByteArray::number(isUnstableResource()));
    urlQuery.addQueryItem("mainVersion", QString("V%1").arg(DSysInfo::majorVersion()));

    url.setQuery(urlQuery);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    qCDebug(DccUpdateWork) << "request url : " << url;
    http->get(request);
}

void UpdateWorker::setUpdateInfo()
{
    m_updatePackages.clear();
    m_systemPackages.clear();
    m_safePackages.clear();
    m_unknownPackages.clear();

    qCDebug(DccUpdateWork) << " UpdateWorker::setUpdateInfo() ";
    m_updatePackages = m_updateInter->classifiedUpdatablePackages();
    m_systemPackages = m_updatePackages.value(SystemUpdateType);
    m_safePackages = m_updatePackages.value(SecurityUpdateType);
    m_unknownPackages = m_updatePackages.value(UnknownUpdateType);

    qCDebug(DccUpdateWork) << "systemUpdate packages:" << m_systemPackages;
    qCDebug(DccUpdateWork) << "safeUpdate packages:" << m_safePackages;
    qCDebug(DccUpdateWork) << "unkonowUpdate packages:" << m_unknownPackages;

    if (m_model->status() == UpdatesStatus::UpdateFailed) {
        qCDebug(DccUpdateWork) << " [UpdateWork] The status is error. Current status : "
                               << m_model->status();
        return;
    }

    int updateCount = m_systemPackages.count() + m_safePackages.count() + m_unknownPackages.count();
    if (updateCount < 1) {
        QFile file("/tmp/.dcc-update-successd");
        if (file.exists()) {
            m_model->setStatus(UpdatesStatus::NeedRestart, __LINE__);
            return;
        }
    }

    // 如果内存中没有日志数据，那么从文件里面读取
    do {
        if (!m_updateLogs.isEmpty() || !QFile::exists(UpdateLogTmpFile))
            break;

        qInfo() << "Update log is empty, read logs from temporary file";
        QFile logFile(UpdateLogTmpFile);
        if (!logFile.open(QFile::ReadOnly)) {
            qWarning() << "Can not open update log file:" << UpdateLogTmpFile;
            break;
        }

        QJsonParseError err_rpt;
        QJsonDocument updateInfoDoc = QJsonDocument::fromJson(logFile.readAll(), &err_rpt);
        logFile.close();
        if (err_rpt.error != QJsonParseError::NoError) {
            qWarning() << "Parse update log error: " << err_rpt.errorString();
            break;
        }
        setUpdateLogs(updateInfoDoc.array());
        qInfo() << "Update logs size: " << m_updateLogs.size();
    } while (0);

    QMap<ClassifyUpdateType, UpdateItemInfo *> updateInfoMap = getAllUpdateInfo();
    m_model->setAllDownloadInfo(updateInfoMap);

    qCDebug(DccUpdateWork) << " UpdateWorker::setUpdateInfo: updateInfoMap.count()"
                           << updateInfoMap.count();

    if (updateInfoMap.count() == 0) {
        m_model->setStatus(UpdatesStatus::Updated, __LINE__);
    } else {
        qCDebug(DccUpdateWork) << "UpdateWorker::setAppUpdateInfo: downloadSize = "
                               << m_downloadSize;
        for (uint type = ClassifyUpdateType::SystemUpdate;
             type <= ClassifyUpdateType::SecurityUpdate;
             type++) {
            ClassifyUpdateType classifyType = uintToclassifyUpdateType(type);
            if (updateInfoMap.contains(classifyType)) {
                if (updateInfoMap.value(classifyType) != nullptr) {
                    m_downloadSize += updateInfoMap.value(classifyType)->downloadSize();
                    if (m_model->getClassifyUpdateStatus(classifyType) != UpdatesStatus::Downloading
                        && m_model->getClassifyUpdateStatus(classifyType)
                                != UpdatesStatus::DownloadPaused
                        && m_model->getClassifyUpdateStatus(classifyType)
                                != UpdatesStatus::Downloaded
                        && m_model->getClassifyUpdateStatus(classifyType)
                                != UpdatesStatus::Installing) {
                        m_model->setClassifyUpdateTypeStatus(classifyType,
                                                             UpdatesStatus::UpdatesAvailable);
                    }
                }
            } else {
                m_model->setClassifyUpdateTypeStatus(classifyType, UpdatesStatus::Default);
            }
        }
        m_model->setStatus(UpdatesStatus::UpdatesAvailable, __LINE__);
    }
}

QMap<ClassifyUpdateType, UpdateItemInfo *> UpdateWorker::getAllUpdateInfo()
{
    qCDebug(DccUpdateWork) << "getAllUpdateInfo";
    QMap<ClassifyUpdateType, UpdateItemInfo *> resultMap;

    QMap<ClassifyUpdateType, QString> updateDailyKeyMap;
    updateDailyKeyMap.insert(ClassifyUpdateType::SystemUpdate, "systemUpdateInfo");
    updateDailyKeyMap.insert(ClassifyUpdateType::SecurityUpdate, "safeUpdateInfo");
    updateDailyKeyMap.insert(ClassifyUpdateType::UnknownUpdate, "otherUpdateInfo");

    qulonglong updateMode = m_model->updateMode();

    if (m_systemPackages.count() > 0 && (updateMode & ClassifyUpdateType::SystemUpdate)) {
        UpdateItemInfo *systemItemInfo = new UpdateItemInfo;
        systemItemInfo->setName(tr("System Updates"));
        systemItemInfo->setExplain(tr("Fixed some known bugs and security vulnerabilities"));
        setUpdateItemDownloadSize(systemItemInfo, m_systemPackages);
        resultMap.insert(ClassifyUpdateType::SystemUpdate, systemItemInfo);
    }

    if (m_safePackages.count() > 0 && (updateMode & ClassifyUpdateType::SecurityUpdate)) {
        UpdateItemInfo *safeItemInfo = new UpdateItemInfo;
        safeItemInfo->setName(tr("Security Updates"));
        safeItemInfo->setExplain(tr("Fixed some known bugs and security vulnerabilities"));
        setUpdateItemDownloadSize(safeItemInfo, m_safePackages);
        resultMap.insert(ClassifyUpdateType::SecurityUpdate, safeItemInfo);
    }

    if (m_unknownPackages.count() > 0 && (updateMode & ClassifyUpdateType::UnknownUpdate)) {
        UpdateItemInfo *unkownItemInfo = new UpdateItemInfo;
        unkownItemInfo->setName(tr("Third-party Repositories"));
        setUpdateItemDownloadSize(unkownItemInfo, m_unknownPackages);
        resultMap.insert(ClassifyUpdateType::UnknownUpdate, unkownItemInfo);
    }

    // 将更新日志根据`系统更新`or`安全更新`进行分类，并保存留用
    for (ClassifyUpdateType type : resultMap.keys()) {
        int logType = -1;
        if (type == ClassifyUpdateType::SecurityUpdate) {
            logType = LogTypeSecurity;
        } else if (type == ClassifyUpdateType::SystemUpdate) {
            logType = LogTypeSystem;
        } else {
            continue;
        }

        UpdateItemInfo *itemInfo = resultMap.value(type);
        if (!itemInfo)
            continue;

        for (UpdateLogItem logItem : m_updateLogs) {
            if (!logItem.isValid() || logItem.logType != logType)
                continue;

            updateItemInfo(logItem, resultMap.value(type));
        }
    }

    return resultMap;
}

void UpdateWorker::getItemInfo(QJsonValue jsonValue, UpdateItemInfo *itemInfo)
{
    if (jsonValue.isNull() || itemInfo == nullptr) {
        return;
    }

    QStringList language = QLocale::system().name().split('_');
    QString languageType = "CN";
    if (language.count() > 1) {
        languageType = language.value(1);
        if (languageType == "CN" || languageType == "TW" || languageType == "HK") {
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
        itemInfo->setUpdateTime(
                jsonValue.toObject().value("update_time_" + languageType).toString());
    } else {
        itemInfo->setUpdateTime(jsonValue.toObject().value("update_time").toString());
    }

    qCDebug(DccUpdateWork) << "UpdateWorker::getItemInfo  itemInfo->name() == " << itemInfo->name();

    QJsonValue dataValue = jsonValue.toObject().value("data_info");
    if (dataValue.isArray()) {
        QJsonArray array = dataValue.toArray();
        QList<DetailInfo> itemList;
        int count = array.count();
        for (int i = 0; i < count; ++i) {
            DetailInfo detailInfo;
            detailInfo.name =
                    array.at(i).toObject().value("name_" + languageType).toString().trimmed();
            detailInfo.updateTime =
                    array.at(i).toObject().value("update_time").toString().trimmed();
            detailInfo.info = array.at(i)
                                      .toObject()
                                      .value("detail_info_" + languageType)
                                      .toString()
                                      .trimmed();
            detailInfo.link = array.at(i).toObject().value("link").toString().trimmed();
            if (detailInfo.name.isEmpty() && detailInfo.updateTime.isEmpty()
                && detailInfo.info.isEmpty() && detailInfo.link.isEmpty()) {
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

    if (!checkJobIsValid(m_checkUpdateJob) || !checkJobIsValid(m_sysUpdateDownloadJob)
        || !checkJobIsValid(m_sysUpdateInstallJob) || !checkJobIsValid(m_safeUpdateDownloadJob)
        || !checkJobIsValid(m_safeUpdateInstallJob) || !checkJobIsValid(m_unknownUpdateDownloadJob)
        || !checkJobIsValid(m_unknownUpdateInstallJob)) {

        return false;
    }

    return true;
}

// 处于以下状态时，就不能再去设置其他更新的状态了，直接显示对应错误提示
bool UpdateWorker::getNotUpdateState()
{
    UpdatesStatus state = m_model->status();

    return state != UpdatesStatus::RecoveryBackupFailed
            && state != UpdatesStatus::RecoveryBackupFailedDiskFull
            && state != UpdatesStatus::UpdateFailed;
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

    m_checkUpdateJob = new UpdateJobDBusProxy(jobPath, this);

    connect(m_checkUpdateJob,
            &UpdateJobDBusProxy::StatusChanged,
            this,
            &UpdateWorker::onCheckUpdateStatusChanged);
    connect(qApp, &QApplication::aboutToQuit, this, [=] {
        if (m_checkUpdateJob) {
            delete m_checkUpdateJob.data();
        }
    });

    connect(m_checkUpdateJob,
            &UpdateJobDBusProxy::ProgressChanged,
            m_model,
            &UpdateModel::setUpdateProgress,
            Qt::QueuedConnection);
    m_checkUpdateJob->ProgressChanged(m_checkUpdateJob->progress());
    m_checkUpdateJob->StatusChanged(m_checkUpdateJob->status());
    ret.jobID = m_checkUpdateJob->id();
    ret.jobDescription = m_checkUpdateJob->description();
    qCDebug(DccUpdateWork) << " Get Job: " << ret.jobID << ret.jobDescription
                           << m_checkUpdateJob->progress() << m_checkUpdateJob->status();
    return ret;
}

void UpdateWorker::distUpgrade(ClassifyUpdateType updateType)
{
    UpdatesStatus status = m_model->getClassifyUpdateStatus(updateType);

    if (m_backupStatus == BackupStatus::Backingup) {
        QPointer<UpdateJobDBusProxy> job = getDownloadJob(updateType);
        if (job != nullptr) {
            m_updateInter->CleanJob(job->id());
            deleteJob(job);
        }
        m_model->setClassifyUpdateTypeStatus(updateType, UpdatesStatus::WaitForRecoveryBackup);
        return;
    }
    if (m_backupStatus == BackupStatus::Backuped) {
        downloadAndInstallUpdates(updateType);
        return;
    }

    if (status == UpdatesStatus::Downloading) {
        QPointer<UpdateJobDBusProxy> job = getDownloadJob(updateType);
        if (job != nullptr) {
            m_updateInter->CleanJob(job->id());
            deleteJob(job);
        }
    }

    m_backupingClassifyType = updateType;
    // 开始进行原子更新  原子更新只有在失败 或者 第二此更新才可以进行备份
    qCDebug(DccUpdateWork) << "distUpgrade:"
                           << " == start Atomic Upgrade == ";
    // 条件不足 1. 分区空间 就是 state = -2    2.  分区格式不支持仓库存储（忽略） 3.
    // 第二更新后的失败更新

    auto var = m_updateConfig->value("backup", true);
    if (var.toBool() && !m_updateInter->atomBackupIsRunning()) {
        backupToAtomicUpgrade();
    } else {
        // 系统环境配置不满足,则直接跳到下一步下载数据
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
    qCDebug(DccUpdateWork) << "setUpdateMode"
                           << "set UpdateMode to dbus:" << updateMode;

    m_updateInter->setUpdateMode(updateMode);
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

void UpdateWorker::setBackupUpdates(const bool &backupUpdates)
{
    m_updateConfig->setValue("backup", backupUpdates);
}

void UpdateWorker::handleUpdateLogsReply(QNetworkReply *reply)
{
    qCInfo(DccUpdateWork) << "Handle reply of update log";
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Network Error" << reply->errorString();
        return;
    }
    QByteArray respondBody = reply->readAll();
    if (respondBody.isEmpty()) {
        qCWarning(DccUpdateWork) << "Request body is empty";
        return;
    }

    const QJsonDocument &doc = QJsonDocument::fromJson(respondBody);
    const QJsonObject &obj = doc.object();
    if (obj.isEmpty()) {
        qWarning() << "Request body json object is empty";
        return;
    }
    if (obj.value("code").toInt() != 0) {
        qWarning() << "Request update log failed";
        return;
    }

    const QJsonArray array = obj.value("data").toArray();
    setUpdateLogs(array);
    // 保存一个临时文件，在没有获取到在线日志的时候展示文件中的内容
    QFile::remove(UpdateLogTmpFile);
    QFile file(UpdateLogTmpFile);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument dataDoc;
        dataDoc.setArray(array);
        file.write(dataDoc.toJson());
        file.close();
    }
}

QString UpdateWorker::getUpdateLogAddress() const
{
    QScopedPointer<DConfig> dconfig(DConfig::create("org.deepin.dde.control-center",
                                                    QStringLiteral("org.deepin.dde.control-center.update")));

    const QString fallbackAddr("https://update-platform.uniontech.com/api/v1/systemupdatelogs");
    if (dconfig) {
        const QString &updateLogAddress = dconfig->value("updateLogAddress", fallbackAddr).toString();
        if (!updateLogAddress.isEmpty()) {
            qCDebug(DccUpdateWork) << " updateLogAddress " << updateLogAddress;
            return updateLogAddress;
        }
    }

    return fallbackAddr;
}

/**
 * @brief 发行版or内测版
 *
 * @return 1: 发行版，2：内测版
 */
int UpdateWorker::isUnstableResource() const
{
    qInfo() << Q_FUNC_INFO;
    const int RELEASE_VERSION = 1;
    const int UNSTABLE_VERSION = 2;
    QScopedPointer<DConfig> config(DConfig::create("org.deepin.unstable", "org.deepin.unstable"));
    if (!config) {
        qInfo() << "Can not find org.deepin.unstable or an error occurred in DTK";
        return RELEASE_VERSION;
    }

    if (!config->keyList().contains("updateUnstable")) {
        qInfo() << "Key(updateUnstable) was not found ";
        return RELEASE_VERSION;
    }

    const QString &value = config->value("updateUnstable", "Enabled").toString();
    qInfo() << "Config(updateUnstable) value: " << value;
    return "Enabled" == value ? UNSTABLE_VERSION : RELEASE_VERSION;
}

void UpdateWorker::setUpdateLogs(const QJsonArray &array)
{
    if (array.isEmpty())
        return;

    m_updateLogs.clear();
    for (const QJsonValue &value : array) {
        QJsonObject obj = value.toObject();
        if (obj.isEmpty())
            continue;

        UpdateLogItem item;
        item.id = obj.value("id").toInt();
        item.systemVersion = obj.value("systemVersion").toString();
        item.cnLog = obj.value("cnLog").toString();
        item.enLog = obj.value("enLog").toString();
        item.publishTime = m_model->utcDateTime2LocalDate(obj.value("publishTime").toString());
        item.platformType = obj.value("platformType").toInt();
        item.serverType = obj.value("serverType").toInt();
        item.logType = obj.value("logType").toInt();
        m_updateLogs.append(std::move(item));
    }
    qInfo() << "m_updateLogs size: " << m_updateLogs.size();
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
    connect(process,
            static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this,
            [this, process](int result, QProcess::ExitStatus) {
                bool isNetselectExist = 0 == result;
                if (!isNetselectExist) {
                    qCDebug(DccUpdateWork)
                            << "[wubw UpdateWorker] netselect 127.0.0.1 : " << isNetselectExist;
                }
                m_model->setNetselectExist(isNetselectExist);
                process->deleteLater();
            });
}

void UpdateWorker::setCheckUpdatesJob(const QString &jobPath)
{
    qCDebug(DccUpdateWork) << "[setCheckUpdatesJob] start status : " << m_model->status();
    UpdatesStatus state = m_model->status();
    if (UpdatesStatus::Downloading != state && UpdatesStatus::DownloadPaused != state
        && UpdatesStatus::Installing != state) {
        m_model->setStatus(UpdatesStatus::Checking, __LINE__);
    } else if (UpdatesStatus::UpdateFailed == state) {
        resetDownloadInfo();
    }

    createCheckUpdateJob(jobPath);
}

void UpdateWorker::setDownloadJob(const QString &jobPath, ClassifyUpdateType updateType)
{
    QMutexLocker locker(&m_downloadMutex);
    if (m_model->status() == UpdatesStatus::Default
        || m_model->status() == UpdatesStatus::Checking) {
        setUpdateInfo();
    }

    m_model->setStatus(UpdatesStatus::Updating, __LINE__);
    QPointer<UpdateJobDBusProxy> job = new UpdateJobDBusProxy(jobPath, this);
    switch (updateType) {
    case ClassifyUpdateType::SystemUpdate:
        m_sysUpdateDownloadJob = job;
        connect(m_sysUpdateDownloadJob,
                &UpdateJobDBusProxy::ProgressChanged,
                this,
                &UpdateWorker::onSysUpdateDownloadProgressChanged);
        connect(m_sysUpdateDownloadJob,
                &UpdateJobDBusProxy::NameChanged,
                this,
                &UpdateWorker::setSysUpdateDownloadJobName);
        break;

    case ClassifyUpdateType::SecurityUpdate:
        m_safeUpdateDownloadJob = job;
        connect(m_safeUpdateDownloadJob,
                &UpdateJobDBusProxy::ProgressChanged,
                this,
                &UpdateWorker::onSafeUpdateDownloadProgressChanged);
        connect(m_safeUpdateDownloadJob,
                &UpdateJobDBusProxy::NameChanged,
                this,
                &UpdateWorker::setSafeUpdateDownloadJobName);
        break;

    case ClassifyUpdateType::UnknownUpdate:
        m_unknownUpdateDownloadJob = job;
        connect(m_unknownUpdateDownloadJob,
                &UpdateJobDBusProxy::ProgressChanged,
                this,
                &UpdateWorker::onUnkonwnUpdateDownloadProgressChanged);
        connect(m_unknownUpdateDownloadJob,
                &UpdateJobDBusProxy::NameChanged,
                this,
                &UpdateWorker::setUnknownUpdateDownloadJobName);
        break;

    default:
        break;
    }

    connect(job, &UpdateJobDBusProxy::StatusChanged, this, [=](QString status) {
        onClassityDownloadStatusChanged(updateType, status);
    });

    job->StatusChanged(job->status());
    job->ProgressChanged(job->progress());
    job->NameChanged(job->name());
}

void UpdateWorker::setDistUpgradeJob(const QString &jobPath, ClassifyUpdateType updateType)
{
    QMutexLocker locker(&m_mutex);
    m_model->setStatus(UpdatesStatus::Updating, __LINE__);
    QPointer<UpdateJobDBusProxy> job = new UpdateJobDBusProxy(jobPath, this);
    switch (updateType) {
    case ClassifyUpdateType::SystemUpdate:
        m_sysUpdateInstallJob = job;
        connect(m_sysUpdateInstallJob,
                &UpdateJobDBusProxy::ProgressChanged,
                this,
                &UpdateWorker::onSysUpdateInstallProgressChanged);
        break;
    case ClassifyUpdateType::SecurityUpdate:
        m_safeUpdateInstallJob = job;
        connect(m_safeUpdateInstallJob,
                &UpdateJobDBusProxy::ProgressChanged,
                this,
                &UpdateWorker::onSafeUpdateInstallProgressChanged);
        break;
    case ClassifyUpdateType::UnknownUpdate:
        m_unknownUpdateInstallJob = job;
        connect(m_unknownUpdateInstallJob,
                &UpdateJobDBusProxy::ProgressChanged,
                this,
                &UpdateWorker::onUnkonwnUpdateInstallProgressChanged);
        break;
    default:
        break;
    }

    connect(job, &UpdateJobDBusProxy::StatusChanged, this, [=](QString status) {
        onClassityInstallStatusChanged(updateType, status);
    });

    job->StatusChanged(job->status());
    job->ProgressChanged(job->progress());
}

void UpdateWorker::setUpdateItemProgress(UpdateItemInfo *itemInfo, double value)
{
    // 异步加载数据,会导致下载信息还未获取就先取到了下载进度
    if (itemInfo) {
        if (!getNotUpdateState()) {
            qCDebug(DccUpdateWork) << " Now can't to update continue...";
            resetDownloadInfo();
            return;
        }
        itemInfo->setDownloadProgress(value);

    } else {
        // 等待下载信息加载后,再通过 onNotifyDownloadInfoChanged()
        // 设置"UpdatesStatus::Downloading"状态
        qCDebug(DccUpdateWork) << "[wubw download] DownloadInfo is nullptr , waitfor download info";
    }
}

void UpdateWorker::setAutoCleanCache(const bool autoCleanCache)
{
    m_updateInter->SetAutoClean(autoCleanCache);
}

void UpdateWorker::onJobListChanged(const QList<QDBusObjectPath> &jobs)
{
    if (!hasRepositoriesUpdates()) {
        return;
    }
    for (const auto &job : jobs) {
        m_jobPath = job.path();

        UpdateJobDBusProxy jobInter(m_jobPath, this);

        // id maybe scrapped
        const QString &id = jobInter.id();
        // 防止刚打开控制中心的时候获取joblist的时候job还存在，由于构建jobInter可能会花销一定时间导致构建完成后job已经完成，这个时候需要设置对应的更新状态为更新成功
        if (id.isEmpty() && !m_jobPath.isEmpty()) {
            if (m_jobPath.contains("system_upgrade")) {
                m_model->setClassifyUpdateTypeStatus(ClassifyUpdateType::SystemUpdate,
                                                     UpdatesStatus::UpdateSucceeded);
            } else if (m_jobPath.contains("security_upgrade")) {
                m_model->setClassifyUpdateTypeStatus(ClassifyUpdateType::SecurityUpdate,
                                                     UpdatesStatus::UpdateSucceeded);
            } else if (m_jobPath.contains("unknown_upgrade")) {
                m_model->setClassifyUpdateTypeStatus(ClassifyUpdateType::UnknownUpdate,
                                                     UpdatesStatus::UpdateSucceeded);
            }
            continue;
        }

        if (!jobInter.isValid())
            continue;

        qCDebug(DccUpdateWork) << "[wubw] onJobListChanged, id : " << id
                               << " , m_jobPath : " << m_jobPath;
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
        } else {
            qCDebug(DccUpdateWork) << "Install id: " + id + ", nothing to do";
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

    qCDebug(DccUpdateWork) << "onUnkonwnUpdateInstallProgressChanged : " << value;
    setUpdateItemProgress(itemInfo, value);
}

void UpdateWorker::onCheckUpdateStatusChanged(const QString &value)
{
    qCDebug(DccUpdateWork) << "[setCheckUpdatesJob]status is: " << value;
    if (value == "failed" || value.isEmpty()) {
        qWarning() << "check for updates job failed";
        if (m_checkUpdateJob != nullptr) {
            m_updateInter->CleanJob(m_checkUpdateJob->id());
            checkDiskSpace(m_checkUpdateJob->description());
            deleteJob(m_checkUpdateJob);
        }
    } else if (value == "success" || value == "succeed") {
        setUpdateInfo();
    } else if (value == "end") {
        deleteJob(m_checkUpdateJob);
        setUpdateInfo();
    }
}

void UpdateWorker::checkDiskSpace(const QString &jobDescription)
{
    qCDebug(DccUpdateWork) << "job description: " << jobDescription;

    m_model->setClassityUpdateJonError(ClassifyUpdateType::Invalid,
                                       analyzeJobErrorMessage(jobDescription));
    m_model->setStatus(UpdatesStatus::UpdateFailed, __LINE__);
    qCDebug(DccUpdateWork) << Q_FUNC_INFO << "UpdateFailed , jobDescription : " << jobDescription;

    // 以上错误均需重置更新信息
    resetDownloadInfo();
}

void UpdateWorker::setBatteryPercentage(const BatteryPercentageInfo &info)
{
    m_batteryPercentage = info.value("Display", 0);
    const bool low = m_onBattery && m_batteryPercentage < 50;
    m_model->setLowBattery(low);
}

// Now D-Bus only in system power have BatteryPercentage data
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
    QPointer<UpdateJobDBusProxy> job = getDownloadJob(type);

    if (job == nullptr) {
        return;
    }

    switch (updateCtrlType) {
    case UpdateCtrlType::Start:
        m_updateInter->StartJob(job->id());
        break;
    case UpdateCtrlType::Pause:
        m_updateInter->PauseJob(job->id());
        break;
    }
}

void UpdateWorker::downloadAndInstallUpdates(ClassifyUpdateType updateType)
{
    uint64_t type = (uint64_t)updateType;
    QDBusPendingCallWatcher *watcher =
            new QDBusPendingCallWatcher(m_updateInter->ClassifiedUpgrade(type), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher, updateType] {
        if (!watcher->isError()) {
            watcher->reply().path();
            QDBusPendingReply<QList<QDBusObjectPath>> reply = watcher->reply();
            QList<QDBusObjectPath> data = reply.value();
            if (data.count() < 1) {
                qCDebug(DccUpdateWork)
                        << "UpdateFailed, download updates error: " << watcher->error().message();
                return;
            }
            setDownloadJob(reply.value().at(0).path(), updateType);
        } else {
            m_model->setClassifyUpdateTypeStatus(updateType, UpdatesStatus::UpdateFailed);
            resetDownloadInfo();
            QPointer<UpdateJobDBusProxy> job = getDownloadJob(updateType);
            if (!job.isNull()) {
                m_updateInter->CleanJob(job->id());
            }

            job = getInstallJob(updateType);
            if (!job.isNull()) {
                m_updateInter->CleanJob(job->id());
            }
            qCDebug(DccUpdateWork)
                    << "UpdateFailed, download updates error: " << watcher->error().message();
        }
    });
}

QPointer<UpdateJobDBusProxy> UpdateWorker::getDownloadJob(ClassifyUpdateType updateType)
{
    QPointer<UpdateJobDBusProxy> job;
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

QPointer<UpdateJobDBusProxy> UpdateWorker::getInstallJob(ClassifyUpdateType updateType)
{
    QPointer<UpdateJobDBusProxy> job;
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

bool UpdateWorker::checkJobIsValid(QPointer<UpdateJobDBusProxy> dbusJob)
{
    if (!dbusJob.isNull()) {
        if (dbusJob->isValid() && getNotUpdateState()) {
            return true;
        } else {
            dbusJob->deleteLater();
            return false;
        }
    }

    return false;
}

void UpdateWorker::deleteJob(QPointer<UpdateJobDBusProxy> dbusJob)
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
    if ((m_model->getSystemUpdateStatus() == UpdatesStatus::UpdateSucceeded
         || m_model->getSystemUpdateStatus() == UpdatesStatus::Default)
        && (m_model->getSafeUpdateStatus() == UpdatesStatus::UpdateSucceeded
            || m_model->getSafeUpdateStatus() == UpdatesStatus::Default)
        && (m_model->getUnkonowUpdateStatus() == UpdatesStatus::UpdateSucceeded
            || m_model->getUnkonowUpdateStatus() == UpdatesStatus::Default)) {
        QFile file("/tmp/.dcc-update-successd");
        if (file.exists())
            return true;
        file.open(QIODevice::WriteOnly);
        file.close();
        return true;
    }

    return false;
}

void UpdateWorker::cleanLastoreJob(QPointer<UpdateJobDBusProxy> dbusJob)
{
    if (dbusJob != nullptr) {
        m_updateInter->CleanJob(dbusJob->id());
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
    qCDebug(DccUpdateWork) << reply.errorMessage();
}

UpdateErrorType UpdateWorker::analyzeJobErrorMessage(QString jobDescription)
{
    QJsonParseError err_rpt;
    QJsonDocument jobErrorMessage = QJsonDocument::fromJson(jobDescription.toUtf8(), &err_rpt);

    if (err_rpt.error != QJsonParseError::NoError) {
        qCDebug(DccUpdateWork) << "更新失败JSON格式错误";
        return UpdateErrorType::NoError;
    }
    const QJsonObject &object = jobErrorMessage.object();
    QString errorType = object.value("ErrType").toString();
    if (errorType.contains("fetchFailed", Qt::CaseInsensitive)
        || errorType.contains("IndexDownloadFailed", Qt::CaseInsensitive)) {
        return UpdateErrorType::NoNetwork;
    }
    if (errorType.contains("unmetDependencies", Qt::CaseInsensitive)
        || errorType.contains("dependenciesBroken", Qt::CaseInsensitive)) {
        return UpdateErrorType::DeependenciesBrokenError;
    }
    if (errorType.contains("insufficientSpace", Qt::CaseInsensitive)) {
        return UpdateErrorType::NoSpace;
    }
    if (errorType.contains("interrupted", Qt::CaseInsensitive)) {
        return UpdateErrorType::DpkgInterrupted;
    }

    return UpdateErrorType::UnKnown;
}

void UpdateWorker::onClassityDownloadStatusChanged(const ClassifyUpdateType type,
                                                   const QString &value)
{
    qCDebug(DccUpdateWork) << "onClassityDownloadStatusChanged ::" << type << "status :: " << value;
    if (value == "running" || value == "ready") {
        m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::Downloading);
    } else if (value == "failed") {
        QPointer<UpdateJobDBusProxy> job = getDownloadJob(type);
        qCDebug(DccUpdateWork) << "onClassityDownloadStatusChanged ::" << type
                               << "job->description() :: " << job->description();
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

void UpdateWorker::onClassityInstallStatusChanged(const ClassifyUpdateType type,
                                                  const QString &value)
{
    qCDebug(DccUpdateWork) << "onClassityInstallStatusChanged ::" << type << "status :: " << value;
    if (value == "ready") {
        m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::Downloaded);
    } else if (value == "running") {
        m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::Installing);
    } else if (value == "failed") {
        QPointer<UpdateJobDBusProxy> job = getInstallJob(type);
        qCDebug(DccUpdateWork) << "onClassityInstallStatusChanged ::" << type
                               << "job->description() :: " << job->description();
        m_model->setClassityUpdateJonError(type, analyzeJobErrorMessage(job->description()));
        m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::UpdateFailed);
        cleanLastoreJob(job);
    } else if (value == "succeed") {
        m_model->setClassifyUpdateTypeStatus(type, UpdatesStatus::UpdateSucceeded);
        // update updateState ==> setBadge(false)
        m_model->isUpdatablePackages(false);
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
    return value;
}

void UpdateWorker::checkUpdatablePackages(const QMap<QString, QStringList> &updatablePackages)
{
    qCDebug(DccUpdateWork) << " ---- UpdatablePackages = " << updatablePackages.count();
    QMap<ClassifyUpdateType, QString> keyMap;
    keyMap.insert(ClassifyUpdateType::SystemUpdate, SystemUpdateType);
    keyMap.insert(ClassifyUpdateType::UnknownUpdate, UnknownUpdateType);
    keyMap.insert(ClassifyUpdateType::SecurityUpdate, SecurityUpdateType);
    bool showUpdateNotify = false;
    for (auto item : keyMap.keys()) {
        if ((m_model->updateMode() & static_cast<unsigned>(item))
            && updatablePackages.value(keyMap.value(item)).count() > UPDATE_PACKAGE_SIZE) {
            showUpdateNotify = true;
            break;
        }
    }
    m_model->isUpdatablePackages(showUpdateNotify);
}

// 可进行原子更新
void UpdateWorker::backupToAtomicUpgrade()
{
    m_model->setStatus(UpdatesStatus::Updating, __LINE__);
    m_model->setClassifyUpdateTypeStatus(m_backupingClassifyType, UpdatesStatus::RecoveryBackingup);
    /*
        "{"SubmissionTime":"1653034897","SystemVersion":"UOS-V23-2000-107","SubmissionType":0,"UUID":"02eb924f-4f35-4880-b839-096c3a65f525","Note":"系统更新"}"
    */
    // 拼接json
    QMap<QString, QVariant> commitDate;
    commitDate.insert("SubmissionTime", m_model->commitSubmissionTime());
    commitDate.insert("SystemVersion", m_model->systemVersion());
    commitDate.insert("SubmissionType", m_model->submissionType());
    commitDate.insert("UUID", m_model->UUID());
    commitDate.insert("Note", "System Update");

    QJsonDocument docCommitDate = QJsonDocument::fromVariant(QVariant(commitDate));
    QJsonObject jsonObj = docCommitDate.object();
    QString strjson = QJsonDocument(jsonObj).toJson(QJsonDocument::Compact);

    // 异步调用 commit
    onAtomicUpdateing();
    m_updateInter->commit(strjson);
}

void UpdateWorker::updateItemInfo(const UpdateLogItem &logItem, UpdateItemInfo *itemInfo)
{
    if (!logItem.isValid() || !itemInfo) {
        return;
    }

    QStringList language = QLocale::system().name().split('_');
    QString languageType = "CN";
    if (language.count() > 1) {
        languageType = language.value(1);
        if (languageType == "CN" || languageType == "TW" || languageType == "HK") {
            languageType = "CN";
        } else {
            languageType = "US";
        }
    }

    // 安全更新只会更新与当前系统版本匹配的内容，例如，105X的系统版本只会更新105X的安全更新，而不会更新106X的
    // 更新日志也需要与之匹配，只显示与当前系统版本相同的安全更新日志
    if (logItem.logType == LogTypeSecurity) {
        const QString &currentSystemVer = IsCommunitySystem ? Dtk::Core::DSysInfo::deepinVersion()
                                                            : Dtk::Core::DSysInfo::minorVersion();
        QString tmpSystemVersion = logItem.systemVersion;
        tmpSystemVersion.replace(tmpSystemVersion.length() - 1, 1, '0');
        if (currentSystemVer.compare(tmpSystemVersion) != 0) {
            return;
        }
    }

    const QString &explain = languageType == "CN" ? logItem.cnLog : logItem.enLog;
    // 写入最近的更新
    if (itemInfo->currentVersion().isEmpty()) {
        itemInfo->setCurrentVersion(logItem.systemVersion);
        itemInfo->setAvailableVersion(logItem.systemVersion);
        itemInfo->setExplain(explain);
        itemInfo->setUpdateTime(logItem.publishTime);
    } else {
        DetailInfo detailInfo;
        const QString &systemVersion = logItem.systemVersion;
        // 专业版不不在详细信息中显示维护线版本
        if (!IsProfessionalSystem || (!systemVersion.isEmpty() && systemVersion.back() == '0')) {
            detailInfo.name = logItem.systemVersion;
            detailInfo.updateTime = logItem.publishTime;
            detailInfo.info = explain;
            itemInfo->addDetailInfo(detailInfo);
        }
    }
}

bool UpdateWorker::hasRepositoriesUpdates()
{
    qulonglong mode = m_model->updateMode();
    return (mode & ClassifyUpdateType::SystemUpdate) || (mode & ClassifyUpdateType::UnknownUpdate)
            || (mode & ClassifyUpdateType::SecurityUpdate);
}

void UpdateWorker::handleAtomicStateChanged(int operate,
                                            int state,
                                            QString version,
                                            QString message)
{
    /*
        Int32 state: 当序⾏结束时状态码;
        1： 当序⾏中;
        0: 成功;
        -1: 存放仓库路径不存在;
        -2: 存放仓库磁盘间不⾜;
        -3: grub更失败;
        -4: 统磁盘挂载或载失败;
        -5: ostree仓库初失败;
        -6: ostree提交本发⽣错误;
        -7: ostree检出本发⽣错误;
        -8: 本作的仓库本不允许除;
        -9: 本作的仓库本不存在;
    */
    qCDebug(DccUpdateWork) << " Atomic State : " << state << "operate: " << operate
                           << " version: " << version << " message: " << message;
    switch (state) {
    case BackupResult::Success:
        m_backupStatus = BackupStatus::Backuped;
        m_model->setClassifyUpdateTypeStatus(m_backupingClassifyType,
                                             UpdatesStatus::RecoveryBackingSuccessed);
        onAtomicUpdateFinshed(true);
        break;
    case BackupResult::BackingUp:
        m_backupStatus = BackupStatus::Backingup;
        onAtomicUpdateing();
        break;
    default:
        m_backupStatus = BackupStatus::BackupFailed;
        if (state == RecoveryBackupFailedDistFull) {
            m_model->setClassifyUpdateTypeStatus(m_backupingClassifyType,
                                                 UpdatesStatus::RecoveryBackupFailedDiskFull);
        } else {
            m_model->setClassifyUpdateTypeStatus(m_backupingClassifyType,
                                                 UpdatesStatus::RecoveryBackupFailed);
        }
        qCDebug(DccUpdateWork) << "handleAtomicStateChanged"
                               << " [Atomic Backup] 备份失败 , message : " << message;
        m_backupStatus = BackupStatus::BackupFailed;
        onAtomicUpdateFinshed(false);
        break;
    }
}

void UpdateWorker::onAtomicUpdateFinshed(bool successed)
{
    auto requestUpdate = [=](ClassifyUpdateType type) -> bool {
        if (m_model->getClassifyUpdateStatus(type) == UpdatesStatus::WaitForRecoveryBackup
            || m_model->getClassifyUpdateStatus(type) == UpdatesStatus::RecoveryBackingup
            || m_model->getClassifyUpdateStatus(type) == UpdatesStatus::RecoveryBackingSuccessed) {
            distUpgrade(type);
            return true;
        }
        return false;
    };
    if (successed) {
        requestUpdate(ClassifyUpdateType::SystemUpdate);
        requestUpdate(ClassifyUpdateType::SecurityUpdate);
        requestUpdate(ClassifyUpdateType::UnknownUpdate);
    } else {
        if (requestUpdate(ClassifyUpdateType::SystemUpdate)
            || requestUpdate(ClassifyUpdateType::SecurityUpdate)
            || requestUpdate(ClassifyUpdateType::UnknownUpdate)) {
            return;
        }
    }
}

void UpdateWorker::onAtomicUpdateing()
{
    // 处理正在备份的状态
    qCDebug(DccUpdateWork) << Q_FUNC_INFO << " [AtomicUpdateing] 可以备份, 开始备份...";
    switch (m_backupingClassifyType) {
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
}

void UpdateWorker::onClassifiedUpdatablePackagesChanged(QMap<QString, QStringList> packages)
{
    m_systemPackages = packages.value(SystemUpdateType);
    if (m_systemPackages.count() == 0) {
        m_model->setClassifyUpdateTypeStatus(ClassifyUpdateType::SystemUpdate,
                                             UpdatesStatus::Default);
    }
    m_safePackages = packages.value(SecurityUpdateType);
    if (m_safePackages.count() == 0) {
        m_model->setClassifyUpdateTypeStatus(ClassifyUpdateType::SecurityUpdate,
                                             UpdatesStatus::Default);
    }
    m_unknownPackages = packages.value(UnknownUpdateType);
    if (m_unknownPackages.count() == 0) {
        m_model->setClassifyUpdateTypeStatus(ClassifyUpdateType::UnknownUpdate,
                                             UpdatesStatus::Default);
    }
    checkUpdatablePackages(packages);
}

void UpdateWorker::onFixError(const ClassifyUpdateType &updateType, const QString &errorType)
{
    m_fixErrorUpdate.append(updateType);
    if (m_fixErrorJob != nullptr) {
        return;
    }
    QDBusInterface lastoreManager("org.deepin.dde.Lastore1",
                                  "/org/deepin/dde/Lastore1",
                                  "org.deepin.dde.Lastore1.Manager",
                                  QDBusConnection::systemBus());
    if (!lastoreManager.isValid()) {
        qCDebug(DccUpdateWork) << "com.deepin.license error ," << lastoreManager.lastError().name();
        return;
    }

    QDBusReply<QDBusObjectPath> reply = lastoreManager.call("FixError", errorType);
    if (reply.isValid()) {
        QString path = reply.value().path();
        m_fixErrorJob = new UpdateJobDBusProxy(path, this);
        connect(m_fixErrorJob, &UpdateJobDBusProxy::StatusChanged, this, [=](const QString status) {
            if (status == "succeed" || status == "failed" || status == "end") {
                qCDebug(DccUpdateWork) << "m_fixErrorJob ---status :" << status;
                for (auto type : m_fixErrorUpdate) {
                    distUpgrade(type);
                }
                m_fixErrorUpdate.clear();
                deleteJob(m_fixErrorJob);
            }
        });
    }
}

void UpdateWorker::setUpdateItemDownloadSize(UpdateItemInfo *updateItem, QStringList packages)
{
    QDBusPendingCall call = m_updateInter->PackagesDownloadSize(packages);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, updateItem, [updateItem, call, watcher] {
        if (!call.isError()) {
            QDBusReply<qlonglong> reply = call.reply();
            qlonglong value = reply.value();
            updateItem->setDownloadSize(value);
        }
        watcher->deleteLater();
    });
}

void UpdateWorker::onRequestLastoreHeartBeat()
{
    QDBusInterface lastoreManager("org.deepin.dde.Lastore1",
                                  "/org/deepin/dde/Lastore1",
                                  "org.deepin.dde.Lastore1.Updater",
                                  QDBusConnection::systemBus());
    if (!lastoreManager.isValid()) {
        qCDebug(DccUpdateWork) << "com.deepin.license error ," << lastoreManager.lastError().name();
        return;
    }
    lastoreManager.asyncCall("GetCheckIntervalAndTime");
}

std::optional<QString> UpdateWorker::getMachineId()
{
    if (m_machineid.has_value()) {
        return m_machineid.value();
    }
    QString machineid = m_updateInter->hardwareId();
    if (!machineid.isEmpty()) {
        m_machineid = machineid;
        return machineid;
    }
    return std::nullopt;
}

std::optional<QUrl> UpdateWorker::updateTestingChannelUrl()
{
    QString hostname = m_updateInter->staticHostname();
    auto machineid = getMachineId();
    if (!machineid.has_value()) {
        return std::nullopt;
    }
    QUrl testingUrl = QUrl(ServiceLink + "/internal-testing");
    auto query = QUrlQuery(testingUrl.query());
    query.addQueryItem("h", hostname);
    query.addQueryItem("m", machineid.value());
    query.addQueryItem("v", DSysInfo::minorVersion());
    testingUrl.setQuery(query);
    return testingUrl;
}

std::optional<QUrl> UpdateWorker::getTestingChannelUrl()
{
    if (!m_testingChannelUrl.has_value()) {
        m_testingChannelUrl = updateTestingChannelUrl();
    }
    return m_testingChannelUrl;
}

std::optional<QString> UpdateWorker::getTestingChannelSource()
{
    auto sourceFile = QString("/etc/apt/sources.list.d/%1.list").arg(TestingChannelPackage);
    qCDebug(DccUpdateWork) << "sourceFile" << sourceFile;
    QFile f(sourceFile);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        return std::nullopt;
    }
    QTextStream in(&f);
    while (!in.atEnd()) {
        auto line = in.readLine();
        if (line.startsWith("deb")) {
            auto fields = line.split(" ", Qt::SkipEmptyParts);
            if (fields.length() >= 2) {
                auto sourceURL = fields[1];
                if (sourceURL.endsWith("/")) {
                    sourceURL.truncate(sourceURL.length() - 1);
                }
                return sourceURL;
            }
        }
    }
    return std::nullopt;
}

// get all sources of the package
QStringList UpdateWorker::getSourcesOfPackage(const QString &pkg, const QString &version)
{
    QStringList sources;
    QProcess aptCacheProcess;
    QStringList args;
    args.append("madison");
    args.append(pkg);
    // exec apt-cache madison $pkg
    aptCacheProcess.start("apt-cache", args);
    aptCacheProcess.waitForFinished();
    while (aptCacheProcess.canReadLine()) {
        QString line(aptCacheProcess.readLine());
        auto fields = line.split("|", Qt::SkipEmptyParts);
        for (QString &field : fields) {
            field = field.trimmed();
        }
        if (fields.length() <= 2) {
            continue;
        }
        auto p = fields[0], ver = fields[1], src = fields[2];
        src.truncate(fields[2].indexOf(" "));
        if (p == pkg) {
            if (version.length() == 0 || version == ver) {
                sources.append(src);
            }
        }
    }
    return sources;
}

CanExitTestingChannelStatus UpdateWorker::checkCanExitTestingChannelDialog()
{
    auto dialog = new DDialog;
    dialog->setFixedWidth(400);
    dialog->setFixedHeight(280);

    auto label = new DLabel(dialog);
    label->setWordWrap(true);
    label->setText(QObject::tr("Checking system versions, please wait..."));

    auto progress = new DWaterProgress(dialog);
    progress->setFixedSize(100, 100);
    progress->setTextVisible(false);
    progress->setValue(50);
    progress->start();

    QWidget *content = new QWidget(dialog);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    content->setLayout(layout);
    dialog->addContent(content);
    dialog->addButton(QObject::tr("Leave"), false, DDialog::ButtonWarning);
    dialog->addButton(QObject::tr("Cancel"), true, DDialog::ButtonRecommend);

    layout->addStretch();
    layout->addWidget(label, 0, Qt::AlignHCenter);
    layout->addSpacing(20);
    layout->addWidget(progress, 0, Qt::AlignHCenter);
    layout->addStretch();
    CanExitTestingChannelStatus wantexit = CanExitTestingChannelStatus::Cancel;
    connect(dialog,
            &DDialog::buttonClicked,
            this,
            [&wantexit, dialog](int index, const QString &text) {
                Q_UNUSED(text)
                if (index == 0) {
                    // clicked the leave button
                    wantexit = CanExitTestingChannelStatus::CheckOk;
                } else {
                    // clicked the cancel button
                    wantexit = CanExitTestingChannelStatus::Cancel;
                }
                dialog->deleteLater();
            });
    dialog->setDisabled(true);

    auto canExit = CanExitTestingChannelStatus::CheckOk;
    auto testChannelSrc = getTestingChannelSource();
    if (!testChannelSrc.has_value()) {
        qCDebug(DccUpdateWork) << "Not have source file";
        return CanExitTestingChannelStatus::CheckError;
    }
    QString testingChannelSource = testChannelSrc.value();

    QProcess dpkgL;
    dpkgL.start("dpkg", { "-l" });
    dpkgL.waitForFinished(-1);

    if (dpkgL.exitStatus() == QProcess::CrashExit) {
        qCWarning(DccUpdateWork) << "run dpkg error:" << dpkgL.readAllStandardError();
        return CanExitTestingChannelStatus::CheckError;
    }

    QString data = dpkgL.readAllStandardOutput();
    QStringList listpkgpre = data.split('\n');

    QList<std::tuple<QString, QString>> listpkg;

    QRegularExpression re(PACKAGE_REGEX);

    for (const QString &line : listpkgpre) {
        QRegularExpressionMatch match = re.match(line);
        if (!match.hasMatch()) {
            continue;
        }
        QString pkg = match.captured(1);
        QString version = match.captured(2);
        // skip non system software
        if (!pkg.contains("dde") && !pkg.contains("deepin") && !pkg.contains("dtk")
            && !pkg.contains("uos")) {
            continue;
        }
        listpkg.push_back({ pkg, version });
    }

    int taskUnitJobs = 20;

    if (qEnvironmentVariableIsSet(CHECK_JOBS_ENV)) {
        auto env = qgetenv(CHECK_JOBS_ENV);
        int settedJobs = env.toInt();
        if (settedJobs > 0) {
            taskUnitJobs = settedJobs;
        }
    }

    int pkglen = listpkg.length() / taskUnitJobs;

    if (listpkg.length() % taskUnitJobs != 0) {
        pkglen += 1;
    }

    QList<QList<std::tuple<QString, QString>>> inputValues;

    for (int i = 0; i < pkglen; i++) {
        int start_pos = i * taskUnitJobs;
        inputValues.push_back(listpkg.mid(start_pos, taskUnitJobs));
    }

    std::function<bool(const QList<std::tuple<QString, QString>> list)> checkCanExit =
            [this, &canExit, testingChannelSource](const QList<std::tuple<QString, QString>> list) {
                for (const auto &[pkg, version] : list) {
                    {
                        std::lock_guard<std::mutex> guard(CHECK_CANEXIST_GUARD);
                        if (canExit != CanExitTestingChannelStatus::CheckOk) {
                            return true;
                        }
                    }

                    // Does the package exists only in the internal test source
                    auto sources = getSourcesOfPackage(pkg, version);
                    if (sources.length() == 1 && sources[0].contains(testingChannelSource)) {
                        std::lock_guard<std::mutex> guard(CHECK_CANEXIST_GUARD);
                        canExit = CanExitTestingChannelStatus::CheckError;
                        return true;
                    }
                }
                return true;
            };
    QFutureWatcher<bool> *check_watcher = new QFutureWatcher<bool>(this);

    QFuture check_future = QtConcurrent::mapped(inputValues, checkCanExit);

    connect(check_watcher,
            &QFutureWatcher<bool>::finished,
            this,
            [check_watcher, dialog, label, &canExit] {
                check_watcher->deleteLater();
                std::lock_guard<std::mutex> guard(CHECK_CANEXIST_GUARD);
                if (canExit == CanExitTestingChannelStatus::CheckError) {
                    label->setText(tr("It may be unsafe for you to leave the internal testing "
                                      "channel now, do you still want to leave?"));
                } else {
                    label->setText(tr("Your are safe to leave the internal testing channel"));
                }
                dialog->setDisabled(false);
            });
    check_watcher->setFuture(check_future);
    dialog->exec();
    return wantexit;
}

void UpdateWorker::setTestingChannelEnable(const bool &enable)
{
    if (enable) {
        // To Join
        m_model->setTestingChannelStatus(TestingChannelStatus::WaitJoined);
    } else {
        // To Leave
        m_model->setTestingChannelStatus(TestingChannelStatus::WaitToLeave);
    }

    auto machineidopt = getMachineId();
    if (!machineidopt.has_value()) {
        notifyErrorWithoutBody(tr("Cannot find machineid"));

        // INFO: 99lastore-token.conf is not generated, need wait for lastore to generating it, if
        // it is not generated for a long time, please post a issue to lastore
        qCInfo(DccUpdateWork)
                << "machineid need to read /etc/apt/apt.conf.d/99lastore-token.conf, the file is "
                   "generated by lastore. Maybe you need wait for the file to be generated.";
        m_model->setTestingChannelStatus(TestingChannelStatus::NotJoined);
        return;
    }

    QString machineid = machineidopt.value();
    const QString server = ServiceLink;

    // every time, clear the machineid in server
    auto http = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setUrl(QUrl(ServiceLink + "/api/v2/public/testing/machine/" + machineid));

    QEventLoop loop;
    connect(http, &QNetworkAccessManager::finished, this, [http, &loop](QNetworkReply *reply) {
        reply->deleteLater();
        http->deleteLater();
        loop.quit();
    });
    http->deleteResource(request);
    loop.exec();

    // Disable Testing Channel
    if (!enable) {
        if (m_updateInter->PackageExists(TestingChannelPackage)) {
            qCDebug(DccUpdateWork) << "Testing:"
                                   << "Uninstall testing channel package";
            auto exitStatus = checkCanExitTestingChannelDialog();
            if (exitStatus == CanExitTestingChannelStatus::CheckOk) {
                QDBusPendingCall call =
                        m_updateInter->RemovePackage("testing Channel", TestingChannelPackage);
                QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
                connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, call] {
                    if (call.isError()) {
                        notifyError(tr("Cannot Uninstall package") + TestingChannelPackage,
                                    call.error().message());
                        qWarning() << "Cannot Uninstall package " << TestingChannelPackage << " :"
                                   << call.error();
                        m_model->setTestingChannelStatus(TestingChannelStatus::Joined);
                        return;
                    }
                    m_model->setTestingChannelStatus(TestingChannelStatus::NotJoined);
                });
            } else {
                if (exitStatus == CanExitTestingChannelStatus::CheckError) {
                    notifyError(tr("Error when exit testingChannel"),
                                tr("try to manually uninstall package") + TestingChannelPackage);
                }
                m_model->setTestingChannelStatus(TestingChannelStatus::Joined);
                qCDebug(DccUpdateWork) << "Cancel to leave testingChannel";
                return;
            }
        }

        return;
    }
    auto testChannelUrlOpt = getTestingChannelUrl();
    if (!testChannelUrlOpt.has_value()) {
        m_model->setTestingChannelStatus(TestingChannelStatus::NotJoined);
        return;
    }
    QUrl testChannelUrl = testChannelUrlOpt.value();
    qCDebug(DccUpdateWork) << "Testing:"
                           << "open join page" << testChannelUrl.toString();
    QDesktopServices::openUrl(testChannelUrl);
    // Loop to check if user hava joined
    QTimer::singleShot(1000, this, &UpdateWorker::checkTestingChannelStatus);
}

void UpdateWorker::checkTestingChannelStatus()
{
    // Leave page
    if (m_model->getTestingChannelStatus() == TestingChannelStatus::DeActive) {
        return;
    }
    if (!m_machineid.has_value()) {
        return;
    }

    qCDebug(DccUpdateWork) << "Testing:"
                           << "check testing join status";
    QString machineid = m_machineid.value();
    auto http = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setUrl(QUrl(ServiceLink + "/api/v2/public/testing/machine/status/" + machineid));
    request.setRawHeader("content-type", "application/json");
    connect(http, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply) {
        reply->deleteLater();
        http->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            qCDebug(DccUpdateWork) << "Testing:"
                                   << "Network Error" << reply->errorString();
            return;
        }
        auto data = reply->readAll();
        qCDebug(DccUpdateWork) << "Testing:"
                               << "machine status body" << data;
        auto doc = QJsonDocument::fromJson(data);
        auto obj = doc.object();
        auto status = obj["data"].toObject()["status"].toString();
        // Exit the loop if switch status is disable
        if (m_model->getTestingChannelStatus() != TestingChannelStatus::WaitJoined) {
            return;
        }
        // If user has joined then install testing source package;
        if (status == "joined") {
            qCDebug(DccUpdateWork) << "Testing:"
                                   << "Install testing channel package";
            QDBusPendingCall call =
                    m_updateInter->InstallPackage("testing Channel", TestingChannelPackage);
            QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
            connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, call] {
                if (call.isError()) {
                    qWarning() << "Cannot install package" << TestingChannelPackage << ": "
                               << call.error();
                    notifyError(tr("Cannot install package") + TestingChannelPackage,
                                call.error().message());
                    m_model->setTestingChannelStatus(TestingChannelStatus::NotJoined);
                    return;
                }
                m_model->setTestingChannelStatus(TestingChannelStatus::Joined);
            });
            return;
        }
        // Run again after sleep
        QTimer::singleShot(5000, this, &UpdateWorker::checkTestingChannelStatus);
    });
    http->get(request);
}

void UpdateWorker::refreshMirrors()
{
    qDebug() << QDir::currentPath();
    QFile file(":/config/mirrors.json");
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
    m_model->setDefaultMirror(m_updateInter->mirrorSource());
}

void UpdateWorker::setSmartMirror(bool enable)
{
    m_updateInter->SetEnable(enable);
}

void UpdateWorker::setMirrorSource(const MirrorInfo &mirror)
{
    m_updateInter->SetMirrorSource(mirror.m_id);
}

void UpdateWorker::testMirrorSpeed()
{
    QList<MirrorInfo> mirrors = m_model->mirrorInfos();

    QStringList urlList;
    for (MirrorInfo& info : mirrors) {
        urlList << info.m_url;
    }

    // reset the data;
    m_model->setMirrorSpeedInfo(QMap<QString, int>());

    QFutureWatcher<int>* watcher = new QFutureWatcher<int>();
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
