#include "updatework.h"

#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>

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

UpdateWork::UpdateWork(UpdateModel* model, QObject *parent)
    : QObject(parent),
      m_model(model),
      m_downloadJob(nullptr),
      m_checkUpdateJob(nullptr),
      m_distUpgradeJob(nullptr),
      m_updateInter(new UpdateInter("com.deepin.lastore", "/com/deepin/lastore", QDBusConnection::systemBus(), this)),
      m_managerInter(new ManagerInter("com.deepin.lastore", "/com/deepin/lastore", QDBusConnection::systemBus(), this)),
      m_powerInter(new PowerInter("com.deepin.daemon.Power", "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this)),
      m_onBattery(true),
      m_batteryPercentage(0),
      m_baseProgress(0)
{
    m_managerInter->setSync(false);
    m_updateInter->setSync(false);
    m_powerInter->setSync(false);

    connect(m_updateInter, &__Updater::AutoDownloadUpdatesChanged, m_model, &UpdateModel::setAutoDownloadUpdates);
    connect(m_updateInter, &__Updater::MirrorSourceChanged, m_model, &UpdateModel::setDefaultMirror);

    connect(m_powerInter, &__Power::OnBatteryChanged, this, &UpdateWork::setOnBattery);
    connect(m_powerInter, &__Power::BatteryPercentageChanged, this, &UpdateWork::setBatteryPercentage);
}

void UpdateWork::activate()
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

    m_model->setAutoDownloadUpdates(m_updateInter->autoDownloadUpdates());
    setOnBattery(m_powerInter->onBattery());
    setBatteryPercentage(m_powerInter->batteryPercentage());
}

void UpdateWork::deactivate()
{

}


void UpdateWork::checkForUpdates()
{
    QDBusPendingCall call = m_managerInter->UpdateSource();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] {
        if (!call.isError()) {
            QDBusReply<QDBusObjectPath> reply = call.reply();
            const QString jobPath = reply.value().path();
            setCheckUpdatesJob(jobPath);
        } else {
            qWarning() << "check for updates error: " << call.error().message();
        }
    });
}

void UpdateWork::distUpgradeDownloadUpdates()
{
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_managerInter->PrepareDistUpgrade(), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher] {
        if (!watcher->isError()) {
            QDBusReply<QDBusObjectPath> reply = watcher->reply();
            setDownloadJob(reply.value().path());
        } else {
            m_model->setStatus(UpdatesStatus::UpdateFailed);
            qWarning() << "download updates error: " << watcher->error().message();
        }
    });
}

void UpdateWork::distUpgradeInstallUpdates()
{
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_managerInter->DistUpgrade(), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher] {
        if (!watcher->isError()) {
            QDBusReply<QDBusObjectPath> reply = watcher->reply();
            setDistUpgradeJob(reply.value().path());
        } else {
            m_model->setStatus(UpdatesStatus::UpdateFailed);
            qWarning() << "install updates error: " << watcher->error().message();
        }
    });
}

void UpdateWork::pauseDownload()
{
    if (m_downloadJob) {
        m_managerInter->PauseJob(m_downloadJob->id());
        m_model->setStatus(UpdatesStatus::DownloadPaused);
    }
}

void UpdateWork::resumeDownload()
{
    if (m_downloadJob) {
        m_managerInter->StartJob(m_downloadJob->id());
        m_model->setStatus(UpdatesStatus::Downloading);
    }
}

void UpdateWork::distUpgrade()
{
    m_baseProgress = 0;
    distUpgradeInstallUpdates();
}

void UpdateWork::downloadAndDistUpgrade()
{
    m_baseProgress = 0.5;
    distUpgradeDownloadUpdates();
}

void UpdateWork::setAutoDownloadUpdates(const bool &autoDownload)
{
    m_updateInter->SetAutoDownloadUpdates(autoDownload);
}

void UpdateWork::setMirrorSource(const MirrorInfo &mirror)
{
    m_updateInter->SetMirrorSource(mirror.m_id);
}

void UpdateWork::testMirrorSpeed()
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

void UpdateWork::setCheckUpdatesJob(const QString &jobPath)
{
    m_model->setStatus(UpdatesStatus::Checking);

    if(m_checkUpdateJob) {
        m_checkUpdateJob->deleteLater();
        m_checkUpdateJob = nullptr;
    }

    m_checkUpdateJob = new JobInter("com.deepin.lastore", jobPath, QDBusConnection::systemBus(), this);
    connect(m_checkUpdateJob, &__Job::StatusChanged, [this] (const QString & status) {
        if (status == "failed") {
            qWarning() << "check for updates job failed";
        } else if (status == "success" || status == "succeed"){
            m_checkUpdateJob->deleteLater();
            m_checkUpdateJob = nullptr;

            QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_updateInter->ApplicationUpdateInfos(QLocale::system().name()), this);
            connect(w, &QDBusPendingCallWatcher::finished, this, &UpdateWork::onAppUpdateInfoFinished);
        }
    });
}

void UpdateWork::setDownloadJob(const QString &jobPath)
{
    if (m_downloadJob) {
        m_downloadJob->deleteLater();
        m_downloadJob = nullptr;
    }

    m_downloadJob = new JobInter("com.deepin.lastore",
                                 jobPath,
                                 QDBusConnection::systemBus(), this);
    m_model->setStatus(UpdatesStatus::Downloading);

    connect(m_downloadJob, &__Job::ProgressChanged, [this] (double value){
        DownloadInfo *info = m_model->downloadInfo();
        if (info) {
            info->setDownloadProgress(value);
        }
        m_model->setUpgradeProgress(value * 0.5);
    });

    connect(m_downloadJob, &__Job::StatusChanged, [this] (const QString &status) {
        if (status == "failed")  {
            m_model->setStatus(UpdatesStatus::UpdateFailed);
            qWarning() << "download updates job failed";
        } else if (status == "success" || status == "succeed") {
            m_downloadJob->deleteLater();
            m_downloadJob = nullptr;

            // install the updates immediately.
            distUpgradeInstallUpdates();
        }
    });
}

void UpdateWork::setDistUpgradeJob(const QString &jobPath)
{
    if (m_distUpgradeJob) {
        m_distUpgradeJob->deleteLater();
        m_distUpgradeJob = nullptr;
    }

    m_distUpgradeJob = new JobInter("com.deepin.lastore",
                                 jobPath,
                                 QDBusConnection::systemBus(), this);
    m_model->setStatus(UpdatesStatus::Installing);

    connect(m_distUpgradeJob, &__Job::ProgressChanged, [this] (double value){
        m_model->setUpgradeProgress(m_baseProgress + (1 - m_baseProgress) * value);
    });

    connect(m_distUpgradeJob, &__Job::StatusChanged, [this] (const QString &status) {
        if (status == "failed")  {
            m_model->setStatus(UpdatesStatus::UpdateFailed);
            qWarning() << "install updates job failed";
        } else if (status == "success" || status == "succeed") {
            m_distUpgradeJob->deleteLater();
            m_distUpgradeJob = nullptr;

            m_model->setStatus(UpdatesStatus::UpdateSucceeded);

            QFile file("/tmp/.dcc-update-successd");
            if (file.exists())
                return;
            file.open(QIODevice::WriteOnly);
            file.close();
        }
    });
}

void UpdateWork::onAppUpdateInfoFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<AppUpdateInfoList> reply = *w;

    if (reply.isError()) {
        qDebug() << "check for updates job success, but infolist failed." << reply.error();
        m_model->setStatus(UpdatesStatus::Updated);
        w->deleteLater();
        return;
    }

    m_updateInter->setSync(true);
    m_managerInter->setSync(true);

    m_updatableApps = m_updateInter->updatableApps();
    m_updatablePackages = m_updateInter->updatablePackages();

    m_updateInter->setSync(false);
    m_managerInter->setSync(false);

    AppUpdateInfoList value = reply.value();
    AppUpdateInfoList infos;

    int pkgCount = m_updatablePackages.count();
    int appCount = value.count();
    bool foundDDEChangelog = false;

    if (!pkgCount && !appCount) {
        QFile file("/tmp/.dcc-update-successd");
        if (file.exists()) {
            m_model->setStatus(UpdatesStatus::NeedRestart);
            return;
        }
    }

    for (AppUpdateInfo &val : reply.value()) {
        const QString currentVer = val.m_currentVersion;
        const QString lastVer = val.m_avilableVersion;
        AppUpdateInfo info = getInfo(val, currentVer, lastVer);

        if (val.m_packageId == "dde" && !info.m_changelog.isEmpty()) {
            infos.prepend(info);
            foundDDEChangelog = true;
        } else {
            infos << info;
            appCount++;
        }
    }

    qDebug() << pkgCount << appCount << foundDDEChangelog;
    if (pkgCount > appCount && !foundDDEChangelog) {
        // If there's no actual package dde update, but there're system patches available,
        // then fake one dde update item.
        AppUpdateInfo dde;
        dde.m_name = "Deepin";
        dde.m_packageId = "dde";
        dde.m_avilableVersion = tr("Patches");
        AppUpdateInfo ddeUpdateInfo = getInfo(dde, "", "");
        if(ddeUpdateInfo.m_changelog.isEmpty())
            ddeUpdateInfo.m_changelog = tr("System patches.");
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

    w->deleteLater();
}

DownloadInfo *UpdateWork::calculateDownloadInfo(const AppUpdateInfoList &list)
{
    const qlonglong size = m_managerInter->PackagesDownloadSize(m_updatablePackages);

    DownloadInfo *ret = new DownloadInfo(size, list);
    return ret;
}

AppUpdateInfo UpdateWork::getInfo(const AppUpdateInfo &packageInfo, const QString &currentVersion, const QString &lastVersion) const
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
        QJsonObject changelog = locale["changelog"].toObject();
        QString versionedChangelog = fetchVersionedChangelog(changelog, info.m_avilableVersion);

        if (!versionedChangelog.isEmpty())
            info.m_changelog = versionedChangelog;
    }

    return info;
}

void UpdateWork::setBatteryPercentage(const BatteryPercentageInfo &info)
{
    m_batteryPercentage = info.value("Display", 0);
    m_model->setLowBattery(m_onBattery && m_batteryPercentage < 50);
}

void UpdateWork::setOnBattery(bool onBattery)
{
    m_onBattery = onBattery;
    m_model->setLowBattery(m_onBattery && m_batteryPercentage < 50);
}

}
}
