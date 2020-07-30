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
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QApplication>

#define MIN_NM_ACTIVE 50
#define UPDATE_PACKAGE_SIZE 0

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
    , m_downloadJob(nullptr)
    , m_checkUpdateJob(nullptr)
    , m_distUpgradeJob(nullptr)
    , m_otherUpdateJob(nullptr)
    , m_lastoresessionHelper(new LastoressionHelper("com.deepin.LastoreSessionHelper", "/com/deepin/LastoreSessionHelper", QDBusConnection::sessionBus(), this))
    , m_updateInter(new UpdateInter("com.deepin.lastore", "/com/deepin/lastore", QDBusConnection::systemBus(), this))
    , m_managerInter(new ManagerInter("com.deepin.lastore", "/com/deepin/lastore", QDBusConnection::systemBus(), this))
    , m_powerInter(new PowerInter("com.deepin.daemon.Power", "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this))
    , m_powerSystemInter(new PowerSystemInter("com.deepin.system.Power", "/com/deepin/system/Power", QDBusConnection::sessionBus(), this))
    , m_networkInter(new Network("com.deepin.daemon.Network", "/com/deepin/daemon/Network", QDBusConnection::sessionBus(), this))
    , m_smartMirrorInter(new SmartMirrorInter("com.deepin.lastore.Smartmirror", "/com/deepin/lastore/Smartmirror", QDBusConnection::systemBus(), this))
    , m_abRecoveryInter(new RecoveryInter("com.deepin.ABRecovery", "/com/deepin/ABRecovery", QDBusConnection::systemBus(), this))
    , m_systemInfoInter(new SystemInfoInter("com.deepin.daemon.SystemInfo", "/com/deepin/daemon/SystemInfo", QDBusConnection::sessionBus(), this))
    , m_iconTheme(new Appearance("com.deepin.daemon.Appearance","/com/deepin/daemon/Appearance",QDBusConnection::sessionBus(), this))
    , m_onBattery(true)
    , m_batteryPercentage(0.0)
    , m_batterySystemPercentage(0.0)
    , m_baseProgress(0.0)
    , m_bDownAndUpdate(false)
    , m_jobPath("")
    , m_downloadProcess(0.0)
    , m_bIsFirstGetDownloadProcess(true)
    , m_downloadSize(0)
    , m_iconThemeState("")
    , m_beginUpdatesJob(false)
    , m_currLangSelector(new LangSelector("com.deepin.daemon.LangSelector",
                                                "/com/deepin/daemon/LangSelector",
                                                QDBusConnection::sessionBus(), this))
{
    m_managerInter->setSync(false);
    m_updateInter->setSync(false);
    m_powerInter->setSync(false);
    m_powerSystemInter->setSync(false);
    m_lastoresessionHelper->setSync(false);
    m_smartMirrorInter->setSync(true, false);
    m_iconTheme->setSync(false);

    QString sVersion;
    if ("zh_CN.UTF-8" != m_currLangSelector->currentLocale() && DSysInfo::DeepinPersonal == DSysInfo::deepinType()) {
         sVersion= QString("%1 %2 %3").arg(DSysInfo::productTypeString().toUpper(),
                                                     DSysInfo::deepinVersion(),
                                                     "Home");
    } else {
        sVersion = QString("%1 %2 %3").arg(DSysInfo::productTypeString().toUpper(),
                                                     DSysInfo::deepinVersion(),
                                                     DSysInfo::deepinTypeDisplayName());
    }

    m_model->setSystemVersionInfo(sVersion);

    connect(m_managerInter, &ManagerInter::JobListChanged, this, &UpdateWorker::onJobListChanged);
    connect(m_managerInter, &ManagerInter::AutoCleanChanged, m_model, &UpdateModel::setAutoCleanCache);

    connect(m_updateInter, &__Updater::AutoDownloadUpdatesChanged, m_model, &UpdateModel::setAutoDownloadUpdates);
    connect(m_updateInter, &__Updater::MirrorSourceChanged, m_model, &UpdateModel::setDefaultMirror);
    connect(m_updateInter, &UpdateInter::AutoCheckUpdatesChanged, m_model, &UpdateModel::setAutoCheckUpdates);
    connect(m_updateInter, &UpdateInter::UpdateNotifyChanged, m_model, &UpdateModel::setUpdateNotify);

    connect(m_powerInter, &__Power::OnBatteryChanged, this, &UpdateWorker::setOnBattery);
    connect(m_powerInter, &__Power::BatteryPercentageChanged, this, &UpdateWorker::setBatteryPercentage);

    // connect(m_powerSystemInter, &__SystemPower::BatteryPercentageChanged, this, &UpdateWorker::setSystemBatteryPercentage);

    connect(m_smartMirrorInter, &SmartMirrorInter::EnableChanged, m_model, &UpdateModel::setSmartMirrorSwitch);
    connect(m_smartMirrorInter, &SmartMirrorInter::serviceValidChanged, this, &UpdateWorker::onSmartMirrorServiceIsValid);
    connect(m_smartMirrorInter, &SmartMirrorInter::serviceStartFinished, this, [ = ] {
        QTimer::singleShot(100, this, [ = ] {
            m_model->setSmartMirrorSwitch(m_smartMirrorInter->enable());
        });
    }, Qt::UniqueConnection);

    connect(m_abRecoveryInter, &RecoveryInter::JobEnd, this, [ = ](const QString & kind, bool success, const QString & errMsg) {
        qDebug() << " [abRecovery] RecoveryInter::JobEnd 备份结果 -> kind : " << kind << " , success : " << success << " , errMsg : " << errMsg;
        //kind 在备份时为 "backup"，在恢复时为 "restore" (此处为备份)
        if ("backup" == kind) {
            //失败:提示失败,不再进行更新进行
            if (!success) {
                m_model->setStatus(UpdatesStatus::RecoveryBackupFailed, __LINE__);
                qWarning() << Q_FUNC_INFO << " [abRecovery] 备份失败 , errMsg : " << errMsg;
                return;
            }

            m_model->setStatus(UpdatesStatus::RecoveryBackingSuccessed, __LINE__);
            m_model->setUpgradeProgress(1.0);

            //开始下载(只有成功才会继续更新)
            //区分 升级/下载并升级
            if (m_bDownAndUpdate) {
                qDebug() << " [abRecovery] 备份成功, 开始下载并更新 ...";
                m_baseProgress = 0.5;
                distUpgradeDownloadUpdates();
            } else {
                qDebug() << " [abRecovery] 备份成功. 开始更新 ...";
                m_baseProgress = 0;
                distUpgradeInstallUpdates();
            }
        }
    });

    connect(m_abRecoveryInter, &RecoveryInter::BackingUpChanged, m_model, &UpdateModel::setRecoverBackingUp);
    connect(m_abRecoveryInter, &RecoveryInter::ConfigValidChanged, m_model, &UpdateModel::setRecoverConfigValid);
    connect(m_abRecoveryInter, &RecoveryInter::RestoringChanged, m_model, &UpdateModel::setRecoverRestoring);
    //预留接口
   //connect(m_dbusActivator, &GrubDbus::LicenseStateChange, m_model, &UpdateModel::setSystemActivation);
    //图片主题
    connect(m_iconTheme, &Appearance::IconThemeChanged, this, &UpdateWorker::onIconThemeChanged);
    m_iconThemeState = m_iconTheme->iconTheme();

    m_model->setRecoverConfigValid(m_abRecoveryInter->configValid());

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    connect(m_lastoresessionHelper, &LastoressionHelper::SourceCheckEnabledChanged, m_model, &UpdateModel::setSourceCheck);
#endif
    setOnBattery(m_powerInter->onBattery());
    setBatteryPercentage(m_powerInter->batteryPercentage());
    // setSystemBatteryPercentage(m_powerSystemInter->batteryPercentage());
    onJobListChanged(m_managerInter->jobList());

#ifndef DISABLE_SYS_UPDATE_MIRRORS
    refreshMirrors();
#endif
    getLicenseState();

    QDBusConnection::systemBus().connect("com.deepin.license",
                                         "/com/deepin/license/Info",
                                         "com.deepin.license.Info",
                                         "LicenseStateChange",
                                         this,
                                         SLOT(licenseStateChangeSlot()));

    QDBusInterface Interface("com.deepin.lastore",
                                 "/com/deepin/lastore",
                                 "com.deepin.lastore.Updater",
                                 QDBusConnection::systemBus());
    if (!Interface.isValid()) {
        qWarning() << "com.deepin.license error ," << Interface.lastError().name();
        return;
    }
    QList<QString> updatablePackages;
    updatablePackages << Interface.property("UpdatablePackages").toStringList();
    qDebug() << "UpdatablePackages = " << updatablePackages.count();
    if (updatablePackages.count() > UPDATE_PACKAGE_SIZE) {
        m_model->isUpdatablePackages(true);
    } else {
        m_model->isUpdatablePackages(false);
    }
}

UpdateWorker::~UpdateWorker()
{

}

void UpdateWorker::licenseStateChangeSlot()
{
    getLicenseState();
}


void UpdateWorker::getLicenseState()
{
    QDBusInterface licenseInfo("com.deepin.license",
                               "/com/deepin/license/Info",
                               "com.deepin.license.Info",
                               QDBusConnection::systemBus());
    if (!licenseInfo.isValid()) {
        qWarning()<< "com.deepin.license error ,"<< licenseInfo.lastError().name();
        return;
    }
    quint32 reply = licenseInfo.property("AuthorizationState").toUInt();
    qDebug() << "Authorization State:" << reply;
    m_model->setSystemActivation(reply);
}

void UpdateWorker::activate()
{
#ifndef DISABLE_SYS_UPDATE_MIRRORS
    refreshMirrors();
#endif
    double interval = 50.0;
    QString checkTime;
    interval = m_updateInter->GetCheckIntervalAndTime(checkTime);
    m_model->setLastCheckUpdateTime(checkTime);
    m_model->setAutoCheckUpdateCircle(static_cast<int>(interval));

    m_model->setAutoCleanCache(m_managerInter->autoClean());
    m_model->setAutoDownloadUpdates(m_updateInter->autoDownloadUpdates());
    m_model->setAutoCheckUpdates(m_updateInter->autoCheckUpdates());
    m_model->setUpdateNotify(m_updateInter->updateNotify());
#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    m_model->setSourceCheck(m_lastoresessionHelper->sourceCheckEnabled());
#endif
    onSmartMirrorServiceIsValid(m_smartMirrorInter->isValid());
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
        if (!call.isError()) {
            QDBusReply<QDBusObjectPath> reply = call.reply();
            const QString jobPath = reply.value().path();
            setCheckUpdatesJob(jobPath);
        } else {
            m_model->setStatus(UpdatesStatus::UpdateFailed, __LINE__);
            resetDownloadInfo();
            if (!m_checkUpdateJob.isNull()) {
                m_managerInter->CleanJob(m_checkUpdateJob->id());
            }
            qWarning() << "UpdateFailed, check for updates error: " << call.error().message();
        }
    });
}

void UpdateWorker::distUpgradeDownloadUpdates()
{
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_managerInter->PrepareDistUpgrade(), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher] {
        if (!watcher->isError()) {
            QDBusReply<QDBusObjectPath> reply = watcher->reply();
            setDownloadJob(reply.value().path());
        } else {
            m_model->setStatus(UpdatesStatus::UpdateFailed, __LINE__);
            resetDownloadInfo();
            if (!m_distUpgradeJob.isNull()) {
                m_managerInter->CleanJob(m_distUpgradeJob->id());
            }
            qWarning() << "UpdateFailed, download updates error: " << watcher->error().message();
        }
    });
}

void UpdateWorker::distUpgradeInstallUpdates()
{
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_managerInter->DistUpgrade(), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher] {
        if (!watcher->isError()) {
            QDBusReply<QDBusObjectPath> reply = watcher->reply();
            setDistUpgradeJob(reply.value().path());
        } else {
            m_model->setStatus(UpdatesStatus::UpdateFailed, __LINE__);
            resetDownloadInfo();
            if (!m_distUpgradeJob.isNull()) {
                m_managerInter->CleanJob(m_distUpgradeJob->id());
            }
            qWarning() << "UpdateFailed, install updates error: " << watcher->error().message();
        }
    });
}

void UpdateWorker::setAppUpdateInfo(const AppUpdateInfoList &list)
{
    m_updateInter->setSync(true);
    m_managerInter->setSync(true);

    m_updatableApps = m_updateInter->updatableApps();
    m_updatablePackages = m_updateInter->updatablePackages();

    m_updateInter->setSync(false);
    m_managerInter->setSync(false);

    AppUpdateInfoList value = list;
    AppUpdateInfoList infos;

    int pkgCount = m_updatablePackages.count();
    int appCount = value.count();

    //其他错误假如被修正后,已经还能再次设置更新状态
    if (m_model->status() == UpdatesStatus::UpdateFailed) {
        qDebug() << " [UpdateWork] The status is error. Current status : " << m_model->status();
        return;
    }

    if (!pkgCount && !appCount) {
        QFile file("/tmp/.dcc-update-successd");
        if (file.exists()) {
            m_model->setStatus(UpdatesStatus::NeedRestart, __LINE__);
            return;
        }
    }

    for (AppUpdateInfo &val : value) {
        const QString currentVer = val.m_currentVersion;
        const QString lastVer = val.m_avilableVersion;
        AppUpdateInfo info = getInfo(val, currentVer, lastVer);

        infos << info;
    }

    qDebug() << pkgCount << appCount;
    if (pkgCount > 0) {
        // If there's no actual package dde update, but there're system patches available,
        // then fake one dde update item.

        auto it = std::find_if(infos.constBegin(), infos.constEnd(), [ = ](const AppUpdateInfo & info) {
            return info.m_packageId == "dde";
        });

        AppUpdateInfo dde;
        if (it == infos.constEnd()) {
            dde = getDDEInfo();
        } else {
            dde = *it;
            infos.removeAt(it - infos.constBegin());
        }

        AppUpdateInfo ddeUpdateInfo = getInfo(dde, dde.m_currentVersion, dde.m_avilableVersion);
        if (ddeUpdateInfo.m_changelog.isEmpty()) {
            ddeUpdateInfo.m_avilableVersion = tr("Patches");
            ddeUpdateInfo.m_changelog = tr("System patches");
        }

//        if(!DCC_NAMESPACE::IsDesktopSystem) {
//            //app updates are not displayed
//            infos.clear();
//        }

        infos.prepend(ddeUpdateInfo);
    }
    qDebug() << " UpdateWorker::setAppUpdateInfo: infos.count()" << infos.count();
    DownloadInfo *result = calculateDownloadInfo(infos);
    m_model->setDownloadInfo(result);

    qDebug() << "updatable packages:" <<  m_updatablePackages << result->appInfos();
    qDebug() << "total download size:" << formatCap(result->downloadSize());
    m_downloadSize = result->downloadSize();

    qDebug() << "UpdateWorker::setAppUpdateInfo:result->appInfos().length() = " << result->appInfos().length();
    if (result->appInfos().length() == 0) {
        m_model->setStatus(UpdatesStatus::Updated, __LINE__);
    } else {
        qDebug() << "UpdateWorker::setAppUpdateInfo: downloadSize = " << result->downloadSize();
        if (result->downloadSize()) {
            qDebug() << "[wubw download] get download process from DBus (0 : UpdatesAvailable), m_downloadProcess : " << m_downloadProcess;
            if (!compareDouble(m_downloadProcess, 0.0)) {
                m_model->setStatus(UpdatesStatus::UpdatesAvailable, __LINE__);
            } else {
                m_model->setUpgradeProgress(m_downloadProcess);
                onNotifyStatusChanged(UpdatesStatus::DownloadPaused);
            }
        } else {
            if (getNotUpdateState()) {
                qDebug() << "UpdateWorker::setAppUpdateInfo: UpdatesStatus::Downloaded";
                m_model->setStatus(UpdatesStatus::Downloaded, __LINE__);
            }
        }
    }
}

bool UpdateWorker::checkDbusIsValid()
{
    //解决已经显示错误的状态信息后，纠正错误，依旧无法再次加载下载安装信息的问题
    bool ret = getNotUpdateState();

    // If DBUS is valid, the check will bee blocked
    if (!m_checkUpdateJob.isNull()) {
        if (m_checkUpdateJob->isValid() && ret) {
            return true;
        } else {
            m_checkUpdateJob->deleteLater();
        }
    }

    if (!m_downloadJob.isNull()) {
        if (m_downloadJob->isValid() && ret) {
            return true;
        } else {
            m_downloadJob->deleteLater();
        }
    }

    if (!m_distUpgradeJob.isNull()) {
        if (m_distUpgradeJob->isValid() && ret) {
            return true;
        } else {
            m_distUpgradeJob->deleteLater();
        }
    }

    if (!m_otherUpdateJob.isNull()) {
        if (m_otherUpdateJob->isValid() && ret) {
            return true;
        } else {
            m_otherUpdateJob->deleteLater();
        }
    }

    return false;
}

void UpdateWorker::onSmartMirrorServiceIsValid(bool isvalid)
{
    m_smartMirrorInter->setSync(false);
    if (isvalid) {
        m_model->setSmartMirrorSwitch(m_smartMirrorInter->enable());
    } else {
        m_smartMirrorInter->startServiceProcess();
    }
}

void UpdateWorker::onNotifyStatusChanged(UpdatesStatus status)
{
    //若还没有收到下载进度,用户就点击了升级,则不再设置DownloadPaused状态,以用户操作为主
    if (UpdatesStatus::RecoveryBackingup == m_model->status()) {
        qWarning() << "[Update] Direct to backup, then to Updating : Not set UpdatesStatus::DownloadPaused.";
    } else {
        //需要满足以下2点,才能设置为DownloadPaused状态
        //1.已经获取到下载数据; 2.获取到下载进度;
        if (UpdatesStatus::DownloadPaused == status) {
            qDebug() << "[Update] UpdatesStatus::DownloadPaused , m_downloadProcess : " << m_downloadProcess;

            //m_downloadProcess=0.0时，表示先收到了暂停，还未收到进度
            if (m_downloadProcess >= 0.0 && m_downloadProcess < 1.0) {
                m_model->setStatus(UpdatesStatus::DownloadPaused, __LINE__);
            } else if (!compareDouble(m_downloadProcess, 1.0)) {
                //切换状态前，判断当前状态
                if (!getNotUpdateState())
                    return;

                //根据下载数据的大小，判断设置状态
                if (m_downloadSize > 0.0) {
                    m_model->setStatus(UpdatesStatus::UpdatesAvailable, __LINE__);
                } else {
                    m_model->setStatus(UpdatesStatus::Downloaded, __LINE__);
                }
            }
        }
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
    m_downloadProcess = 0.0;
    m_downloadSize = 0;
    m_updatableApps.clear();
    m_updatablePackages.clear();
    if (m_updateInter) {
        setAppUpdateInfo(m_updateInter->ApplicationUpdateInfos(QLocale::system().name()));
    }

    if (!state) {
        if (!m_downloadJob.isNull()) {
            m_downloadJob->deleteLater();
        }

        if (!m_distUpgradeJob.isNull()) {
            m_distUpgradeJob->deleteLater();
        }
    }
}

CheckUpdateJobRet UpdateWorker::createCheckUpdateJob(const QString &jobPath)
{
    CheckUpdateJobRet ret;
    ret.status = "failed";

    QPointer<JobInter> checkUpdateJob = new JobInter("com.deepin.lastore", jobPath, QDBusConnection::systemBus(), this);

    ret.jobID = checkUpdateJob->id();
    ret.jobDescription = checkUpdateJob->description();

    connect(checkUpdateJob, &__Job::StatusChanged, [ &ret, checkUpdateJob ](const QString & status) {
        qDebug() << "[setCheckUpdatesJob]status is: " << status;
        if (status == "failed" || status.isEmpty()) {
            qWarning() << "check for updates job failed";
            ret.status = "failed";
            if (checkUpdateJob) {
                delete checkUpdateJob.data();
            }
        } else if (status == "success" || status == "succeed") {
            ret.status = "succeed";
            if (checkUpdateJob) {
                delete checkUpdateJob.data();
            }
        }
    });

    connect(qApp, &QApplication::aboutToQuit, this, [ = ] {
        if (checkUpdateJob) {
            delete checkUpdateJob.data();
        }
    });

    connect(checkUpdateJob, &__Job::ProgressChanged, m_model, &UpdateModel::setUpdateProgress,Qt::QueuedConnection);
    checkUpdateJob->ProgressChanged(checkUpdateJob->progress());
    checkUpdateJob->StatusChanged(checkUpdateJob->status());

    while (checkUpdateJob) {
        qApp->processEvents();
        QThread::msleep(10);
    }

    return  ret;
}

void UpdateWorker::pauseDownload()
{
    if (!m_downloadJob.isNull()) {
        m_managerInter->PauseJob(m_downloadJob->id());
        onNotifyStatusChanged(UpdatesStatus::DownloadPaused);
    } else {
        qWarning() << "m_downloadJob is nullptr";
    }
}

void UpdateWorker::resumeDownload()
{
    if (!m_downloadJob.isNull()) {
        m_managerInter->StartJob(m_downloadJob->id());
        m_model->setStatus(UpdatesStatus::Downloading, __LINE__);
    } else {
        qWarning() << "m_downloadJob is nullptr";
    }
}

void UpdateWorker::distUpgrade()
{
    if (m_bDownAndUpdate)
        m_bDownAndUpdate = false;

    //First start backupRecovery , then to load(in RecoveryInter::JobEnd Lemon function)
    bool bConfigVlid = m_model->recoverConfigValid();
    qDebug() << Q_FUNC_INFO << " [abRecovery] 更新前,检查备份配置是否满足(true:满足) : " << bConfigVlid;

    if (bConfigVlid) { //系统环境配置为可以恢复,在收到jobEnd()后,且"backup",成功,后才会继续到下一步下载数据
        recoveryCanBackup();
    } else { //系统环境配置不满足,则直接跳到下一步下载数据
        qDebug() << Q_FUNC_INFO << " [abRecovery] 备份配置环境不满足,继续更新.";
        m_baseProgress = 0;
        distUpgradeInstallUpdates();
    }
}

void UpdateWorker::downloadAndDistUpgrade()
{
    if (!m_bDownAndUpdate)
        m_bDownAndUpdate = true;

    bool bConfigVlid = m_model->recoverConfigValid();
    qDebug() << Q_FUNC_INFO << " [abRecovery] 下载并更新前,检查备份配置是否满足(true:满足) : " << bConfigVlid;

    if (bConfigVlid) { //系统环境配置为可以恢复,在收到jobEnd()后,且"backup",成功,后才会继续到下一步下载数据
        recoveryCanBackup();
    } else { //系统环境配置不满足,则直接跳到下一步下载数据
        qDebug() << Q_FUNC_INFO << " [abRecovery] 备份配置环境不满足,继续下载并更新.";
        m_baseProgress = 0.5;
        distUpgradeDownloadUpdates();
    }
}

void UpdateWorker::setAutoCheckUpdates(const bool autocheckUpdates)
{
    m_updateInter->SetAutoCheckUpdates(autocheckUpdates);
}

void UpdateWorker::setAutoDownloadUpdates(const bool &autoDownload)
{
    m_updateInter->SetAutoDownloadUpdates(autoDownload);
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

    QFutureWatcher<int> *watcher = new QFutureWatcher<int>(this);
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
        bool isNetselectExist = false;
        if (result == 0) {
            isNetselectExist = true;
        } else {
            isNetselectExist = false;
        }
        if (!isNetselectExist) {
            qWarning() << "[wubw UpdateWorker] netselect 127.0.0.1 : " << isNetselectExist;
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
    QDBusPendingCall call = m_updateInter->ListMirrorSources(QLocale::system().name());
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] {
        if (!call.isError()) {
            QDBusReply<MirrorInfoList> reply = call.reply();
            MirrorInfoList list  = reply.value();
            m_model->setMirrorInfos(list);
        } else {
            qWarning() << "list mirror sources error: " << call.error().message();
        }
    });

    m_model->setDefaultMirror(m_updateInter->mirrorSource());
}
#endif

void UpdateWorker::recoveryCanBackup()
{
    qDebug() << Q_FUNC_INFO << " [abRecovery] 开始检查是否能备份... ";
    QDBusPendingCall call = m_abRecoveryInter->CanBackup();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] {
        if (!call.isError()) {
            QDBusReply<bool> reply = call.reply();
            bool value = reply.value();
            m_model->setRecoverBackingUp(value);
            if (value) {
                qDebug() << Q_FUNC_INFO << " [abRecovery] 可以备份, 开始备份...";
                m_model->setUpgradeProgress(0.7);
                m_model->setStatus(UpdatesStatus::RecoveryBackingup, __LINE__);
                m_abRecoveryInter->StartBackup();
            } else {
                m_model->setStatus(UpdatesStatus::RecoveryBackupFailed, __LINE__);
                qWarning() << Q_FUNC_INFO << " [abRecovery] 是否能备份(CanBackup)的环境不满足 -> 备份失败 ";
            }
        } else {
            qWarning() << " [abRecovery] recovery CanBackup error: " << call.error().message();
        }
    });
}

void UpdateWorker::recoveryStartRestore()
{
    m_abRecoveryInter->StartRestore();
}

void UpdateWorker::onNotifyDownloadInfoChanged()
{
    qDebug() << "[wubw download] get download info , then to set UpdatesStatus::Downloading status. status , m_downloadProcess : " << m_model->status() << m_downloadProcess;

    DownloadInfo *info = m_model->downloadInfo();
    if (info) {
        if (info->downloadSize() > 0) {
            onNotifyStatusChanged(UpdatesStatus::DownloadPaused);
            info->setDownloadProgress(m_downloadProcess);
            m_model->setUpgradeProgress(m_downloadProcess);
        } else {
            qWarning() << "[UpdateWorker] downloadSize is 0.";
        }
    } else {
        qWarning() << "[UpdateWorker] DownloadInfo is nullptr.";
    }
}

void UpdateWorker::setCheckUpdatesJob(const QString &jobPath)
{
    if (m_beginUpdatesJob)
        return;

    m_beginUpdatesJob = true;
    qDebug() << "[setCheckUpdatesJob] start status : " << m_model->status();
    UpdatesStatus state = m_model->status();
    if (UpdatesStatus::Downloading != state && UpdatesStatus::DownloadPaused != state && UpdatesStatus::Installing != state) {
        m_model->setStatus(UpdatesStatus::Checking, __LINE__);
    } else if (UpdatesStatus::UpdateFailed == state) {
        resetDownloadInfo();
    }

    QFutureWatcher<CheckUpdateJobRet> *watcher = new QFutureWatcher<CheckUpdateJobRet>(this);
    connect(watcher, &QFutureWatcher<CheckUpdateJobRet>::finished, [this, watcher] {
        CheckUpdateJobRet ret = watcher->result();

        if (ret.status == "succeed") {
            QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_updateInter->ApplicationUpdateInfos(QLocale::system().name()), this);
            connect(w, &QDBusPendingCallWatcher::finished, this, &UpdateWorker::onAppUpdateInfoFinished);
        } else {
            m_managerInter->CleanJob(ret.jobID);
            checkDiskSpace(ret.jobDescription);
        }
        m_beginUpdatesJob = false;
        watcher->deleteLater();
    });

    QFuture<CheckUpdateJobRet> future = QtConcurrent::run(this,&UpdateWorker::createCheckUpdateJob,jobPath);
    watcher->setFuture(future);
}

void UpdateWorker::setDownloadJob(const QString &jobPath)
{
    if (!m_downloadJob.isNull())
        return;

    setAppUpdateInfo(m_updateInter->ApplicationUpdateInfos(QLocale::system().name()));

    m_downloadJob = new JobInter("com.deepin.lastore",
                                 jobPath,
                                 QDBusConnection::systemBus(), this);

    connect(m_downloadJob, &__Job::ProgressChanged, [this](double value) {
        qDebug() << "[wubw download] m_downloadJob, value : " << value << m_bIsFirstGetDownloadProcess;
        //防止退出后再次进入不确定当前升级的状态,设置正在下载中.
        //假如dbus一直收不到该信号,还是会存在从check直接调到结果的问题(此时就是底层的问题了)
        m_downloadProcess = value;
        DownloadInfo *info = m_model->downloadInfo();
        //异步加载数据,会导致下载信息还未获取就先取到了下载进度
        if (info) {
            if (!getNotUpdateState()) {
                qWarning() << " Now can't to update continue...";
                resetDownloadInfo();
                return;
            }

            //第一次收到下载进度显示暂定,之后再次收到显示更新中
            if (m_bIsFirstGetDownloadProcess) {
                //只有当进度为0的时候,才会显示一次暂停
                if (!compareDouble(m_downloadProcess, 0.0)) {
                    m_bIsFirstGetDownloadProcess = false;
                    onNotifyStatusChanged(UpdatesStatus::DownloadPaused);
                }
            } else {
                if (m_downloadSize > 0) {
                    m_model->setStatus(UpdatesStatus::Downloading, __LINE__);
                } else {
                    qWarning() << " m_downloadSize is 0 : do nothing.";
                }
            }
            info->setDownloadProgress(value);
            m_model->setUpgradeProgress(value);
        } else {
            //等待下载信息加载后,再通过 onNotifyDownloadInfoChanged() 设置"UpdatesStatus::Downloading"状态
            qWarning() << "[wubw download] DownloadInfo is nullptr , waitfor download info";
        }
    });

    connect(m_downloadJob, &__Job::StatusChanged, this, &UpdateWorker::onDownloadStatusChanged);

    m_downloadJob->StatusChanged(m_downloadJob->status());
    m_downloadJob->ProgressChanged(m_downloadJob->progress());
}

void UpdateWorker::setDistUpgradeJob(const QString &jobPath)
{
    if (!m_distUpgradeJob.isNull())
        return;

    setAppUpdateInfo(m_updateInter->ApplicationUpdateInfos(QLocale::system().name()));

    m_distUpgradeJob = new JobInter("com.deepin.lastore",
                                    jobPath,
                                    QDBusConnection::systemBus(), this);

    connect(m_distUpgradeJob, &__Job::ProgressChanged, [this](double value) {
        qDebug() << "[wubw distUpgrade] Update, value : " << value << m_model->status();

        //防止退出后再次进入不确定当前升级的状态,设置正在更新中.
        //假如dbus一直收不到该信号,还是会存在从check直接调到结果的问题(此时就是底层的问题了)
        if (getNotUpdateState()) {
            m_model->setStatus(UpdatesStatus::Installing, __LINE__);
        }
        m_model->setUpgradeProgress(m_baseProgress + (1 - m_baseProgress) * value);
    });

    connect(m_distUpgradeJob, &__Job::StatusChanged, this, &UpdateWorker::onUpgradeStatusChanged);

    if (getNotUpdateState()) {
        m_model->setStatus(UpdatesStatus::Installing, __LINE__);
    }

    m_distUpgradeJob->ProgressChanged(m_distUpgradeJob->progress());
    m_distUpgradeJob->StatusChanged(m_distUpgradeJob->status());
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

        if (!jobInter.isValid())
            continue;

        // id maybe scrapped
        const QString &id = jobInter.id();

        qDebug() << "[wubw] onJobListChanged, id : " << id << " , m_jobPath : " << m_jobPath;
        if (id == "update_source") {
            QTimer::singleShot(0, this, [this]() {
                setCheckUpdatesJob(m_jobPath);
            });
        } else if (id == "prepare_dist_upgrade") {
            QTimer::singleShot(0, this, [this]() {
                setDownloadJob(m_jobPath);
            });
        } else if (id == "dist_upgrade") {
            QTimer::singleShot(0, this, [this]() {
                setDistUpgradeJob(m_jobPath);
            });
        }
    }
}

void UpdateWorker::onAppUpdateInfoFinished(QDBusPendingCallWatcher *w)
{
    qDebug() << "onAppUpdateInfoFinished";
    QDBusPendingReply<AppUpdateInfoList> reply = *w;

    if (reply.isError()) {
        const QJsonObject &obj = QJsonDocument::fromJson(reply.
                                                         error().
                                                         message().
                                                         toUtf8())
                                 .object();

        if (obj["Type"].toString().contains("dependenciesBroken")) {
            m_model->setStatus(UpdatesStatus::DeependenciesBrokenError, __LINE__);
        } else {
            m_model->setStatus(UpdatesStatus::UpdateFailed, __LINE__);
            qWarning() << Q_FUNC_INFO << "UpdateFailed, error msg : " << obj["Type"].toString();
        }

        if (m_model->status() == UpdatesStatus::UpdateFailed) {
            resetDownloadInfo();
        }
        w->deleteLater();
        return;
    }

    setAppUpdateInfo(reply.value());

    w->deleteLater();
}

void UpdateWorker::onDownloadStatusChanged(const QString &status)
{
    qDebug() << "download: <<<" << status;
    if (status == "failed")  {
        m_managerInter->CleanJob(m_downloadJob->id());

        checkDiskSpace(m_downloadJob->description());

        qWarning() << "download updates job failed";
        m_downloadJob->deleteLater();
    } else if (status == "success" || status == "succeed") {
        m_downloadJob->deleteLater();

        // install the updates immediately.
        if (!m_model->autoDownloadUpdates()) {
            qDebug() << "m_model->autoDownloadUpdates()=" << m_model->autoDownloadUpdates();
            distUpgradeInstallUpdates();
        } else {
            // lastore not have download dbus
            qDebug() << "autoDownloadUpdates is open";
            QTimer::singleShot(0, this, [ = ] {
                qDebug() << "m_model->downloadInfo()->downloadSize()=" << m_model->downloadInfo()->downloadSize();
                if (m_model->downloadInfo()->downloadSize()) {
                    checkForUpdates();
                } else {
                    m_model->setStatus(UpdatesStatus::Downloaded, __LINE__);
                }
            });
        }
    } else if (status == "paused") {
        onNotifyStatusChanged(UpdatesStatus::DownloadPaused);
    } else if (status == "running") {
        m_model->setStatus(UpdatesStatus::Downloading, __LINE__);
    }
}

void UpdateWorker::onUpgradeStatusChanged(const QString &status)
{
    qDebug() << "upgrade: <<<" << status;
    if (status == "failed")  {
        // cleanup failed job
        m_managerInter->CleanJob(m_distUpgradeJob->id());

        checkDiskSpace(m_distUpgradeJob->description());

        qWarning() << "install updates job failed";
        m_distUpgradeJob->deleteLater();
    } else if (status == "success" || status == "succeed") {
        m_distUpgradeJob->deleteLater();

        m_model->setStatus(UpdatesStatus::UpdateSucceeded, __LINE__);
        //更新完成,重置下载进度
        resetDownloadInfo();

        QProcess::startDetached("/usr/lib/dde-control-center/reboot-reminder-dialog");

        QFile file("/tmp/.dcc-update-successd");
        if (file.exists())
            return;
        file.open(QIODevice::WriteOnly);
        file.close();
    }
}

void UpdateWorker::onIconThemeChanged(const QString &theme)
{
    m_iconThemeState = theme;
}

void UpdateWorker::checkDiskSpace(const QString &jobDescription)
{
    qDebug() << "job description: " << jobDescription;
    if (jobDescription.contains("You don't have enough free space", Qt::CaseInsensitive) ||
            !m_lastoresessionHelper->IsDiskSpaceSufficient()) {
        m_model->setStatus(UpdatesStatus::NoSpace, __LINE__);
    } else if (jobDescription.contains("Temporary failure resolving", Qt::CaseInsensitive)) {
        m_model->setStatus(UpdatesStatus::NoNetwork, __LINE__);
    } else if (jobDescription.contains("The following packages have unmet dependencies", Qt::CaseInsensitive)) {
        m_model->setStatus(UpdatesStatus::DeependenciesBrokenError, __LINE__);
    } else {
        m_model->setStatus(UpdatesStatus::UpdateFailed, __LINE__);
        qWarning() << Q_FUNC_INFO << "UpdateFailed , jobDescription : " << jobDescription;
    }
    //以上错误均需重置更新信息
    if (m_model->status() == UpdatesStatus::UpdateFailed) {
        resetDownloadInfo();
    }
}

DownloadInfo *UpdateWorker::calculateDownloadInfo(const AppUpdateInfoList &list)
{
    const qlonglong size = m_managerInter->PackagesDownloadSize(m_updatablePackages);

    DownloadInfo *ret = new DownloadInfo(size, list);
    return ret;
}

AppUpdateInfo UpdateWorker::getInfo(const AppUpdateInfo &packageInfo, const QString &currentVersion, const QString &lastVersion) const
{
    auto fetchVersionedChangelog = [](QJsonObject changelog, QString & destVersion) {

        for (QString version : changelog.keys()) {
            if (version == destVersion || destVersion != "") {
                qDebug() << Q_FUNC_INFO << QString("The destVersion(%1) is not empty or version(%2) to the destVersion(%1) to be updated").arg(destVersion).arg(version);
                return changelog.value(version).toString();
            }
        }

        return QStringLiteral("");
    };

    QString metadataDir = "/lastore/metadata/" + packageInfo.m_packageId;
    QString icondataDir = "/usr/share/icons";

    AppUpdateInfo info;
    info.m_packageId = packageInfo.m_packageId;
    info.m_name = packageInfo.m_name;
    info.m_currentVersion = currentVersion;
    info.m_avilableVersion = lastVersion;
    info.m_icon = m_iconThemeState;
    QFile manifest(metadataDir + "/meta/manifest.json");
    if (manifest.open(QFile::ReadOnly)) {
        QByteArray data = manifest.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject object = doc.object();

        info.m_changelog = fetchVersionedChangelog(object["changelog"].toObject(), info.m_avilableVersion);

        QJsonObject locales = object["locales"].toObject();
        QJsonObject locale = locales[QLocale::system().name()].toObject();
        if (locale.isEmpty())
            locale = locales["en_US"].toObject();
        QJsonObject changelog = locale["changelog"].toObject();
        QString versionedChangelog = fetchVersionedChangelog(changelog, info.m_avilableVersion);

        if (!versionedChangelog.isEmpty())
            info.m_changelog = versionedChangelog;
    }

    return info;
}

AppUpdateInfo UpdateWorker::getDDEInfo()
{
    AppUpdateInfo dde;
    dde.m_name = tr("System");
    dde.m_packageId = "dde";

    QFile file("/var/lib/lastore/update_infos.json");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        const QJsonArray array = QJsonDocument::fromJson(file.readAll()).array();
        for (const QJsonValue &json : array) {
            const QJsonObject &obj = json.toObject();
            if (obj["Package"].toString() == "dde") {

                dde.m_currentVersion = obj["CurrentVersion"].toString();
                dde.m_avilableVersion = obj["LastVersion"].toString();

                return dde;
            }
        }
    }

    return dde;
}

void UpdateWorker::setBatteryPercentage(const BatteryPercentageInfo &info)
{
    m_batteryPercentage = info.value("Display", 0);
    const bool low = m_onBattery ? m_batteryPercentage < 50 : false;
    m_model->setLowBattery(low);
}

//Now D-Bus only in system power have BatteryPercentage data
void UpdateWorker::setSystemBatteryPercentage(const double &value)
{
    m_batterySystemPercentage = value;
    const bool low = m_onBattery ? m_batterySystemPercentage < 50 : false;
    m_model->setLowBattery(low);
}

void UpdateWorker::setOnBattery(bool onBattery)
{
    m_onBattery = onBattery;
    const bool low = m_onBattery ? m_batteryPercentage < 50 : false;
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
    double interval = 50.0;
    QString checkTime;
    interval = m_updateInter->GetCheckIntervalAndTime(checkTime);

    m_model->setAutoCheckUpdateCircle(static_cast<int>(interval));
    m_model->setLastCheckUpdateTime(checkTime);
}

void UpdateWorker::setUpdateNotify(const bool notify)
{
    m_updateInter->SetUpdateNotify(notify);

    if (!notify) {
        setAutoDownloadUpdates(false);
    }
}

}
}
