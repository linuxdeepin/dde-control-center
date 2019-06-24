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

#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>

#define MIN_NM_ACTIVE 50

namespace dcc{
namespace update{

static int TestMirrorSpeedInternal(const QString &url)
{
    QStringList args;
    args << url << "-s" << "1";

    QProcess process;
    process.start("netselect", args);
    process.waitForFinished();

    const QString output = process.readAllStandardOutput().trimmed();
    const QStringList result = output.split(' ');

    qDebug() << "speed of url" << url << "is" << result.first();

    if (!result.first().isEmpty()) {
        return result.first().toInt();
    }

    return 10000;
}

UpdateWorker::UpdateWorker(UpdateModel* model, QObject *parent)
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
    , m_networkInter(new Network("com.deepin.daemon.Network", "/com/deepin/daemon/Network", QDBusConnection::sessionBus(), this))
    , m_smartMirrorInter(new SmartMirrorInter("com.deepin.lastore.Smartmirror", "/com/deepin/lastore/Smartmirror", QDBusConnection::systemBus(), this))
    , m_onBattery(true)
    , m_batteryPercentage(0)
    , m_baseProgress(0)
{
    m_managerInter->setSync(false);
    m_updateInter->setSync(false);
    m_powerInter->setSync(false);
    m_lastoresessionHelper->setSync(false);
    m_smartMirrorInter->setSync(true, false);

    connect(m_managerInter, &ManagerInter::JobListChanged, this, &UpdateWorker::onJobListChanged);
    connect(m_managerInter, &ManagerInter::AutoCleanChanged, m_model, &UpdateModel::setAutoCleanCache);

    connect(m_updateInter, &__Updater::AutoDownloadUpdatesChanged, m_model, &UpdateModel::setAutoDownloadUpdates);
    connect(m_updateInter, &__Updater::MirrorSourceChanged, m_model, &UpdateModel::setDefaultMirror);
    connect(m_updateInter, &UpdateInter::AutoCheckUpdatesChanged, m_model, &UpdateModel::setAutoCheckUpdates);

    connect(m_powerInter, &__Power::OnBatteryChanged, this, &UpdateWorker::setOnBattery);
    connect(m_powerInter, &__Power::BatteryPercentageChanged, this, &UpdateWorker::setBatteryPercentage);
    connect(m_smartMirrorInter, &SmartMirrorInter::EnableChanged, m_model, &UpdateModel::setSmartMirrorSwitch);
    connect(m_smartMirrorInter, &SmartMirrorInter::serviceValidChanged, this, &UpdateWorker::onSmartMirrorServiceIsValid);
    connect(m_smartMirrorInter, &SmartMirrorInter::serviceStartFinished, this, [=] {
        QTimer::singleShot(100, this, [=] {
            m_model->setSmartMirrorSwitch(m_smartMirrorInter->enable());
        });
    }, Qt::UniqueConnection);

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    connect(m_lastoresessionHelper, &LastoressionHelper::SourceCheckEnabledChanged, m_model, &UpdateModel::setSourceCheck);
#endif
    setOnBattery(m_powerInter->onBattery());
    setBatteryPercentage(m_powerInter->batteryPercentage());
    onJobListChanged(m_managerInter->jobList());
}

void UpdateWorker::activate()
{
#ifndef DISABLE_SYS_UPDATE_MIRRORS
    refreshMirrors();
#endif

    m_model->setAutoCleanCache(m_managerInter->autoClean());
    m_model->setAutoDownloadUpdates(m_updateInter->autoDownloadUpdates());
    m_model->setAutoCheckUpdates(m_updateInter->autoCheckUpdates());
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
            m_model->setStatus(UpdatesStatus::UpdateFailed);
            if (!m_checkUpdateJob.isNull()) {
                m_managerInter->CleanJob(m_checkUpdateJob->id());
            }
            qWarning() << "check for updates error: " << call.error().message();
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
            m_model->setStatus(UpdatesStatus::UpdateFailed);
            if (!m_distUpgradeJob.isNull()) {
                m_managerInter->CleanJob(m_distUpgradeJob->id());
            }
            qWarning() << "download updates error: " << watcher->error().message();
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
            m_model->setStatus(UpdatesStatus::UpdateFailed);
            if (!m_distUpgradeJob.isNull()) {
                m_managerInter->CleanJob(m_distUpgradeJob->id());
            }
            qWarning() << "install updates error: " << watcher->error().message();
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

    if (!pkgCount && !appCount) {
        QFile file("/tmp/.dcc-update-successd");
        if (file.exists()) {
            m_model->setStatus(UpdatesStatus::NeedRestart);
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
    if (pkgCount > appCount) {
        // If there's no actual package dde update, but there're system patches available,
        // then fake one dde update item.

        auto it = std::find_if(infos.constBegin(), infos.constEnd(), [=] (const AppUpdateInfo &info) {
            return info.m_packageId == "dde";
        });

        AppUpdateInfo dde;
        if (it == infos.constEnd()) {
            dde = getDDEInfo();
        }
        else {
            dde = *it;
            infos.removeAt(it - infos.constBegin());
        }

        AppUpdateInfo ddeUpdateInfo = getInfo(dde, dde.m_currentVersion, dde.m_avilableVersion);
        if(ddeUpdateInfo.m_changelog.isEmpty()) {
            ddeUpdateInfo.m_avilableVersion = tr("Patches");
            ddeUpdateInfo.m_changelog = tr("System patches");
        }
        infos.prepend(ddeUpdateInfo);
    }

    DownloadInfo *result = calculateDownloadInfo(infos);
    m_model->setDownloadInfo(result);

    qDebug() << "updatable packages:" <<  m_updatablePackages << result->appInfos();
    qDebug() << "total download size:" << formatCap(result->downloadSize());

    if (result->appInfos().length() == 0) {
        m_model->setStatus(UpdatesStatus::Updated);
    } else {
        if (result->downloadSize()) {
            m_model->setStatus(UpdatesStatus::UpdatesAvailable);
        } else {
            m_model->setStatus(UpdatesStatus::Downloaded);
        }
    }
}

bool UpdateWorker::checkDbusIsValid()
{
    // If DBUS is valid, the check will bee blocked
    if (!m_checkUpdateJob.isNull()) {
        if (m_checkUpdateJob->isValid()) {
            return true;
        } else {
            m_checkUpdateJob->deleteLater();
        }
    }

    if (!m_downloadJob.isNull()) {
        if (m_downloadJob->isValid()) {
            return true;
        } else {
            m_downloadJob->deleteLater();
        }
    }

    if (!m_distUpgradeJob.isNull()) {
        if (m_distUpgradeJob->isValid()) {
            return true;
        } else {
            m_distUpgradeJob->deleteLater();
        }
    }

    if (!m_otherUpdateJob.isNull()) {
        if (m_otherUpdateJob->isValid()) {
            return true;
        } else {
            m_otherUpdateJob->deleteLater();
        }
    }

    return false;
}

void UpdateWorker::onSmartMirrorServiceIsValid(bool isvalid)
{
    if (isvalid) {
        m_model->setSmartMirrorSwitch(m_smartMirrorInter->enable());
    }
    else {
        m_smartMirrorInter->startServiceProcess();
    }
}

void UpdateWorker::pauseDownload()
{
    if (!m_downloadJob.isNull()) {
        m_managerInter->PauseJob(m_downloadJob->id());
        m_model->setStatus(UpdatesStatus::DownloadPaused);
    }
}

void UpdateWorker::resumeDownload()
{
    if (!m_downloadJob.isNull()) {
        m_managerInter->StartJob(m_downloadJob->id());
        m_model->setStatus(UpdatesStatus::Downloading);
    }
}

void UpdateWorker::distUpgrade()
{
    m_baseProgress = 0;
    distUpgradeInstallUpdates();
}

void UpdateWorker::downloadAndDistUpgrade()
{
    m_baseProgress = 0.5;
    distUpgradeDownloadUpdates();
}

void UpdateWorker::setAutoCheckUpdates(const bool autocheckUpdates)
{
    m_updateInter->SetAutoCheckUpdates(autocheckUpdates);

    if (!autocheckUpdates) {
        setAutoDownloadUpdates(false);
    }
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
    m_model->setMirrorSpeedInfo(QMap<QString,int>());

    QFutureWatcher<int> *watcher = new QFutureWatcher<int>(this);
    connect(watcher, &QFutureWatcher<int>::resultReadyAt, [this, urlList, watcher, mirrors] (int index) {
        QMap<QString, int> speedInfo = m_model->mirrorSpeedInfo();

        int result = watcher->resultAt(index);
        QString mirrorId = mirrors.at(index).m_id;
        speedInfo[mirrorId] = result;

        m_model->setMirrorSpeedInfo(speedInfo);
    });

    QFuture<int> future = QtConcurrent::mapped(urlList, TestMirrorSpeedInternal);
    watcher->setFuture(future);
}

void UpdateWorker::checkNetselect()
{
    QProcess *process = new QProcess;

    const bool isNetselectExist = process->execute("netselect", QStringList() << "127.0.0.1") == 0;

    m_model->setNetselectExist(isNetselectExist);

    process->deleteLater();
}

void UpdateWorker::setSmartMirror(bool enable)
{
    m_smartMirrorInter->SetEnable(enable);

    QTimer::singleShot(100, this, [=] {
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

void UpdateWorker::setCheckUpdatesJob(const QString &jobPath)
{
    if (!m_checkUpdateJob.isNull())
        return;

    m_model->setStatus(UpdatesStatus::Checking);

    m_checkUpdateJob = new JobInter("com.deepin.lastore", jobPath, QDBusConnection::systemBus(), this);
    connect(m_checkUpdateJob, &__Job::StatusChanged, [this] (const QString & status) {
        if (status == "failed") {
            qWarning() << "check for updates job failed";
            m_managerInter->CleanJob(m_checkUpdateJob->id());

            checkDiskSpace(m_checkUpdateJob);

            m_checkUpdateJob->deleteLater();
        } else if (status == "success" || status == "succeed"){
            m_checkUpdateJob->deleteLater();

            QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_updateInter->ApplicationUpdateInfos(QLocale::system().name()), this);
            connect(w, &QDBusPendingCallWatcher::finished, this, &UpdateWorker::onAppUpdateInfoFinished);
        }
    });

    connect(m_checkUpdateJob, &__Job::ProgressChanged, m_model, &UpdateModel::setUpdateProgress);

    m_checkUpdateJob->ProgressChanged(m_checkUpdateJob->progress());
    m_checkUpdateJob->StatusChanged(m_checkUpdateJob->status());
}

void UpdateWorker::setDownloadJob(const QString &jobPath)
{
    if (!m_downloadJob.isNull())
        return;

    setAppUpdateInfo(m_updateInter->ApplicationUpdateInfos(QLocale::system().name()));

    m_downloadJob = new JobInter("com.deepin.lastore",
                                 jobPath,
                                 QDBusConnection::systemBus(), this);

    connect(m_downloadJob, &__Job::ProgressChanged, [this] (double value){
        DownloadInfo *info = m_model->downloadInfo();
        info->setDownloadProgress(value);
        m_model->setUpgradeProgress(value);
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

    connect(m_distUpgradeJob, &__Job::ProgressChanged, [this] (double value){
        m_model->setUpgradeProgress(m_baseProgress + (1 - m_baseProgress) * value);
    });

    connect(m_distUpgradeJob, &__Job::StatusChanged, this, &UpdateWorker::onUpgradeStatusChanged);

    m_model->setStatus(UpdatesStatus::Installing);

    m_distUpgradeJob->ProgressChanged(m_distUpgradeJob->progress());
    m_distUpgradeJob->StatusChanged(m_distUpgradeJob->status());
}

void UpdateWorker::setAutoCleanCache(const bool autoCleanCache)
{
    m_managerInter->SetAutoClean(autoCleanCache);
}

void UpdateWorker::onJobListChanged(const QList<QDBusObjectPath> & jobs)
{
    for (const auto &job : jobs)
    {
        const QString &path = job.path();
        qDebug() << path;

        JobInter jobInter("com.deepin.lastore", path, QDBusConnection::systemBus());

        if (!jobInter.isValid())
            continue;

        // id maybe scrapped
        const QString &id = jobInter.id();

        if (id == "update_source")
            setCheckUpdatesJob(path);
        else if (id == "prepare_dist_upgrade")
            setDownloadJob(path);
        else if (id == "dist_upgrade")
            setDistUpgradeJob(path);
    }
}

void UpdateWorker::onAppUpdateInfoFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<AppUpdateInfoList> reply = *w;

    if (reply.isError()) {
        const QJsonObject &obj = QJsonDocument::fromJson(reply.
                                                         error().
                                                         message().
                                                         toUtf8())
                                 .object();

        if (obj["Type"].toString().contains("dependenciesBroken")) {
            m_model->setStatus(UpdatesStatus::DeependenciesBrokenError);
        }
        else {
            m_model->setStatus(UpdatesStatus::UpdateFailed);
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

        checkDiskSpace(m_downloadJob);

        qWarning() << "download updates job failed";
        m_downloadJob->deleteLater();
    } else if (status == "success" || status == "succeed") {
        m_downloadJob->deleteLater();

        // install the updates immediately.
        if (!m_model->autoDownloadUpdates())
            distUpgradeInstallUpdates();
        else {
            // lastore not have download dbus
            QTimer::singleShot(0, this, [=] {
                if (m_model->downloadInfo()->downloadSize()) {
                    checkForUpdates();
                }
                else {
                    m_model->setStatus(UpdatesStatus::Downloaded);
                }
            });
        }
    } else if (status == "paused") {
        m_model->setStatus(UpdatesStatus::DownloadPaused);
    } else if (status == "running") {
        m_model->setStatus(UpdatesStatus::Downloading);
    }
}

void UpdateWorker::onUpgradeStatusChanged(const QString &status)
{
    qDebug() << "upgrade: <<<" << status;
    if (status == "failed")  {
        // cleanup failed job
        m_managerInter->CleanJob(m_distUpgradeJob->id());

        checkDiskSpace(m_distUpgradeJob);

        qWarning() << "install updates job failed";
        m_distUpgradeJob->deleteLater();
    } else if (status == "success" || status == "succeed") {
        m_distUpgradeJob->deleteLater();

        m_model->setStatus(UpdatesStatus::UpdateSucceeded);

        QProcess::startDetached("/usr/lib/dde-control-center/reboot-reminder-dialog");

        QFile file("/tmp/.dcc-update-successd");
        if (file.exists())
            return;
        file.open(QIODevice::WriteOnly);
        file.close();
    }
}

void UpdateWorker::checkDiskSpace(JobInter *job)
{
    if (job->description().contains("You don't have enough free space") ||
            !m_lastoresessionHelper->IsDiskSpaceSufficient()) {
        m_model->setStatus(UpdatesStatus::NoSpace);
    } else {
        m_model->setStatus(UpdatesStatus::UpdateFailed);
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
            if (version == destVersion) {
                return changelog.value(version).toString();
            }
        }

        return QStringLiteral("");
    };

    QString metadataDir = "/lastore/metadata/" + packageInfo.m_packageId;

    AppUpdateInfo info;
    info.m_packageId = packageInfo.m_packageId;
    info.m_name = packageInfo.m_name;
    info.m_currentVersion = currentVersion;
    info.m_avilableVersion = lastVersion;
    info.m_icon = metadataDir + "/meta/icons/" + packageInfo.m_packageId + ".svg";

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
    dde.m_name = "Deepin";
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

void UpdateWorker::setOnBattery(bool onBattery)
{
    m_onBattery = onBattery;
    const bool low = m_onBattery ? m_batteryPercentage < 50 : false;
    m_model->setLowBattery(low);
}

}
}
