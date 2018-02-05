/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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
    : QObject(parent),
      m_model(model),
      m_downloadJob(nullptr),
      m_checkUpdateJob(nullptr),
      m_distUpgradeJob(nullptr),
      m_otherUpdateJob(nullptr),
      m_lastoresessionHelper(new LastoressionHelper("com.deepin.LastoreSessionHelper", "/com/deepin/LastoreSessionHelper", QDBusConnection::sessionBus(), this)),
      m_updateInter(new UpdateInter("com.deepin.lastore", "/com/deepin/lastore", QDBusConnection::systemBus(), this)),
      m_managerInter(new ManagerInter("com.deepin.lastore", "/com/deepin/lastore", QDBusConnection::systemBus(), this)),
      m_powerInter(new PowerInter("com.deepin.daemon.Power", "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this)),
      m_networkInter(new Network("com.deepin.daemon.Network", "/com/deepin/daemon/Network", QDBusConnection::sessionBus(), this)),
      m_onBattery(true),
      m_batteryPercentage(0),
      m_baseProgress(0)
{
    m_managerInter->setSync(false);
    m_updateInter->setSync(false);
    m_powerInter->setSync(false);
    m_lastoresessionHelper->setSync(false);

    connect(m_managerInter, &ManagerInter::JobListChanged, this, &UpdateWorker::onJobListChanged);
    connect(m_managerInter, &ManagerInter::AutoCleanChanged, m_model, &UpdateModel::setAutoCleanCache);

    connect(m_updateInter, &__Updater::AutoDownloadUpdatesChanged, m_model, &UpdateModel::setAutoDownloadUpdates);
    connect(m_updateInter, &__Updater::MirrorSourceChanged, m_model, &UpdateModel::setDefaultMirror);

    connect(m_powerInter, &__Power::OnBatteryChanged, this, &UpdateWorker::setOnBattery);
    connect(m_powerInter, &__Power::BatteryPercentageChanged, this, &UpdateWorker::setBatteryPercentage);

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    connect(m_lastoresessionHelper, &LastoressionHelper::SourceCheckEnabledChanged, m_model, &UpdateModel::setSourceCheck);
#endif

    onJobListChanged(m_managerInter->jobList());
}

void UpdateWorker::activate()
{
#ifndef DISABLE_SYS_UPDATE_MIRRORS
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
#endif

    m_model->setAutoCleanCache(m_managerInter->autoClean());
    m_model->setAutoDownloadUpdates(m_updateInter->autoDownloadUpdates());
    setOnBattery(m_powerInter->onBattery());
    setBatteryPercentage(m_powerInter->batteryPercentage());
#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    m_model->setSourceCheck(m_lastoresessionHelper->sourceCheckEnabled());
#endif
}

void UpdateWorker::deactivate()
{

}

void UpdateWorker::checkForUpdates()
{
    if (m_checkUpdateJob || m_downloadJob || m_distUpgradeJob)
        return;

    if (m_networkInter->state() <= MIN_NM_ACTIVE) {
        qWarning() << "no network, please check network connect.";
        m_model->setStatus(UpdatesStatus::NoNetwork);
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
            m_managerInter->CleanJob(m_checkUpdateJob->id());
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
            m_managerInter->CleanJob(m_distUpgradeJob->id());
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
            m_managerInter->CleanJob(m_distUpgradeJob->id());
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

        AppUpdateInfo dde = getDDEInfo();

        AppUpdateInfo ddeUpdateInfo = getInfo(dde, dde.m_currentVersion, dde.m_avilableVersion);
        if(ddeUpdateInfo.m_changelog.isEmpty()) {
            ddeUpdateInfo.m_avilableVersion = tr("Patches");
            ddeUpdateInfo.m_changelog = tr("System patches.");
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

void UpdateWorker::pauseDownload()
{
    if (m_downloadJob) {
        m_managerInter->PauseJob(m_downloadJob->id());
        m_model->setStatus(UpdatesStatus::DownloadPaused);
    }
}

void UpdateWorker::resumeDownload()
{
    if (m_downloadJob) {
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

void UpdateWorker::setCheckUpdatesJob(const QString &jobPath)
{
    if (m_checkUpdateJob)
        return;

    m_model->setStatus(UpdatesStatus::Checking);

    m_checkUpdateJob = new JobInter("com.deepin.lastore", jobPath, QDBusConnection::systemBus(), this);
    connect(m_checkUpdateJob, &__Job::StatusChanged, [this] (const QString & status) {
        if (status == "failed") {
            qWarning() << "check for updates job failed";
            m_managerInter->CleanJob(m_checkUpdateJob->id());
            m_model->setStatus(UpdatesStatus::UpdateFailed);
            m_checkUpdateJob->deleteLater();
            m_checkUpdateJob = nullptr;
        } else if (status == "success" || status == "succeed"){
            m_checkUpdateJob->deleteLater();
            m_checkUpdateJob = nullptr;

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
    if (m_downloadJob)
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
    if (m_distUpgradeJob)
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

void UpdateWorker::setOtherUpdate(const QString &jobPath)
{
    if (m_otherUpdateJob)
        return;

    m_otherUpdateJob = new JobInter("com.deepin.lastore",
                                 jobPath,
                                 QDBusConnection::systemBus(), this);

    connect(m_otherUpdateJob, &JobInter::StatusChanged, this, [=] (const QString &status) {
        if (status == "succeed") {
            checkForUpdates();
            m_otherUpdateJob->deleteLater();
            m_otherUpdateJob = nullptr;
        }
    });

    m_otherUpdateJob->StatusChanged(m_otherUpdateJob->status());
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
        else
            setOtherUpdate(path);

    }
}

void UpdateWorker::onAppUpdateInfoFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<AppUpdateInfoList> reply = *w;

    if (reply.isError()) {
        qDebug() << "check for updates job success, but infolist failed." << reply.error();
        m_model->setStatus(UpdatesStatus::Updated);
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
        m_model->setStatus(UpdatesStatus::UpdateFailed);
        qWarning() << "download updates job failed";
        m_downloadJob->deleteLater();
        m_downloadJob = nullptr;
    } else if (status == "success" || status == "succeed") {
        m_downloadJob->deleteLater();
        m_downloadJob = nullptr;

        // install the updates immediately.
        if (!m_model->autoDownloadUpdates())
            distUpgradeInstallUpdates();
        else
            checkForUpdates();
    } else if (status == "paused") {
        m_model->setStatus(UpdatesStatus::DownloadPaused);
    } else {
        m_model->setStatus(UpdatesStatus::Downloading);
    }
}

void UpdateWorker::onUpgradeStatusChanged(const QString &status)
{
    qDebug() << "upgrade: <<<" << status;
    if (status == "failed")  {
        // cleanup failed job
        m_managerInter->CleanJob(m_distUpgradeJob->id());
        m_model->setStatus(UpdatesStatus::UpdateFailed);
        qWarning() << "install updates job failed";
        m_distUpgradeJob->deleteLater();
        m_distUpgradeJob = nullptr;
    } else if (status == "success" || status == "succeed") {
        m_distUpgradeJob->deleteLater();
        m_distUpgradeJob = nullptr;

        m_model->setStatus(UpdatesStatus::UpdateSucceeded);

        QProcess::startDetached("/usr/lib/dde-control-center/reboot-reminder-dialog");

        QFile file("/tmp/.dcc-update-successd");
        if (file.exists())
            return;
        file.open(QIODevice::WriteOnly);
        file.close();
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
