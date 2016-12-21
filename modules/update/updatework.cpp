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
    process.waitForFinished(5000);
    process.kill();

    const QString output = process.readAllStandardOutput().trimmed();
    const QStringList result = output.split(' ');

    qDebug() << "speed of url" << url << "is" << result.first();

    if (!result.first().isEmpty()) {
        return result.first().toInt();
    }

    return -1;
}

UpdateWork::UpdateWork(UpdateModel* model, QObject *parent)
    : QObject(parent),
      m_model(model),
      m_downloadJob(nullptr),
      m_checkUpdateJob(nullptr),
      m_updateInter(new UpdateInter("com.deepin.lastore", "/com/deepin/lastore", QDBusConnection::systemBus(), this)),
      m_managerInter(new ManagerInter("com.deepin.lastore", "/com/deepin/lastore", QDBusConnection::systemBus(), this)),
      m_powerInter(new PowerInter("com.deepin.daemon.Power", "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this))
{
    m_managerInter->setSync(false);
    m_updateInter->setSync(false);
    m_powerInter->setSync(false);

    connect(m_updateInter, &__Updater::AutoCheckUpdatesChanged, m_model, &UpdateModel::setAutoUpdate);
    connect(m_updateInter, &__Updater::MirrorSourceChanged, m_model, &UpdateModel::setDefaultMirror);
}

void UpdateWork::activate()
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

    m_model->setAutoUpdate(m_updateInter->autoCheckUpdates());
    m_model->setDefaultMirror(m_updateInter->mirrorSource());
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

void UpdateWork::downloadUpdates()
{
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_managerInter->PrepareDistUpgrade(), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher] {
        if (!watcher->isError()) {
            QDBusReply<QDBusObjectPath> reply = watcher->reply();
            setDownloadJob(reply.value().path());
        } else {
            qWarning() << "download updates error: " << watcher->error().message();
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

void UpdateWork::setAutoUpdate(const bool &autoUpdate)
{
    m_updateInter->SetAutoCheckUpdates(autoUpdate);
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

    QFutureWatcher<int> *watcher = new QFutureWatcher<int>(this);
    connect(watcher, &QFutureWatcher<int>::finished, [this, urlList, watcher, mirrors] {
        QMap<QString, int> speedInfo;

        for (int i = 0; i < urlList.length(); i++) {
            int result = watcher->resultAt(i);
            QString mirrorId = mirrors.at(i).m_id;
            speedInfo[mirrorId] = result;
        }

        m_model->setMirrorSpeedInfo(speedInfo);
    });

    QFuture<int> future = QtConcurrent::mapped(urlList, TestMirrorSpeedInternal);
    watcher->setFuture(future);
}

void UpdateWork::setCheckUpdatesJob(const QString &jobPath)
{
    if(m_checkUpdateJob) {
        m_checkUpdateJob->deleteLater();
        m_checkUpdateJob = nullptr;
    }

    m_checkUpdateJob = new JobInter("com.deepin.lastore", jobPath, QDBusConnection::systemBus(), this);
    connect(m_checkUpdateJob, &__Job::StatusChanged, [this] (const QString & status) {
        if (status == "failed") {
            qWarning() << "check for updates job failed";
        } else {
            m_checkUpdateJob->deleteLater();
            m_checkUpdateJob = nullptr;

            QFutureWatcher<DownloadInfo*> *watcher = new QFutureWatcher<DownloadInfo*>(this);
            connect(watcher, &QFutureWatcher<DownloadInfo*>::finished, [this, watcher] {
                DownloadInfo *result = watcher->result();
                if (result->downloadSize()) {
                    m_model->setDownloadInfo(result);
                    m_model->setStatus(UpdatesStatus::UpdatesAvailable);
                } else {
                    m_model->setStatus(UpdatesStatus::Downloaded);
                }
            });

            QFuture<DownloadInfo*> future = QtConcurrent::run(this, &UpdateWork::calculateDownloadInfo);
            watcher->setFuture(future);
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
    });

    connect(m_downloadJob, &__Job::StatusChanged, [this] (const QString &status) {
        if (status == "failed")  {
            qWarning() << "download updates job failed";
        } else {
            m_downloadJob->deleteLater();
            m_downloadJob = nullptr;

            m_model->setStatus(UpdatesStatus::Downloaded);
        }
    });
}

DownloadInfo *UpdateWork::calculateDownloadInfo()
{
    m_updateInter->setSync(true);
    m_managerInter->setSync(true);

    m_updatableApps = m_updateInter->updatableApps();
    m_updatablePackages = m_updateInter->updatablePackages();

    QList<AppUpdateInfo> infos = getInfoList();
    const qlonglong size = m_managerInter->PackagesDownloadSize(m_updatablePackages);

    m_updateInter->setSync(false);
    m_managerInter->setSync(false);

    DownloadInfo *ret = new DownloadInfo(size, infos);
    return ret;
}

AppUpdateInfo UpdateWork::getInfo(const QString &packageName, const QString &currentVersion, const QString &lastVersion) const
{

    auto compareVersion = [](QString version1, QString version2) {
        if (version1.isEmpty() || version2.isEmpty()) return false;

        QProcess p;
        p.setArguments(QStringList() << "/usr/bin/dpkg" << "--compare-versions" << version1 << "gt" << version2);
        p.waitForFinished();
        return p.exitCode() == 0;
    };

    auto fetchVersionedChangelog = [compareVersion](QJsonObject changelog, QString & currentVersion) {
        QString result;

        for (QString version : changelog.keys()) {
            if (compareVersion(version, currentVersion)) {
                if (result.isNull() || result.isEmpty()) {
                    result = result + changelog.value(version).toString();
                } else {
                    result = result + '\n' + changelog.value(version).toString();
                }
            }
        }

        return result;
    };

    QString metadataDir = "/lastore/metadata/" + packageName;

    AppUpdateInfo info;
    info.m_packageId = packageName;
    info.m_currentVersion = currentVersion;
    info.m_avilableVersion = lastVersion;
    info.m_icon = metadataDir + "/meta/icons/" + packageName + ".svg";

    QFile manifest(metadataDir + "/meta/manifest.json");
    if (manifest.open(QFile::ReadOnly)) {
        QByteArray data = manifest.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject object = doc.object();

        info.m_name = object["name"].toString();
        info.m_changelog == fetchVersionedChangelog(object["changelog"].toObject(), info.m_currentVersion);

        QJsonObject locales = object["locales"].toObject();
        QJsonObject locale = locales[QLocale::system().name()].toObject();
        QJsonObject changelog = locale["changelog"].toObject();
        QString versionedChangelog = fetchVersionedChangelog(changelog, info.m_currentVersion);

        if (!locale["name"].toString().isEmpty())
            info.m_name = locale["name"].toString();
        if (!versionedChangelog.isEmpty())
            info.m_changelog = versionedChangelog;
    }

    return info;
}

QList<AppUpdateInfo> UpdateWork::getInfoList()
{
    QList<AppUpdateInfo> infos;
    QFile updateInfos("/var/lib/lastore/update_infos.json");
    if (updateInfos.open(QFile::ReadOnly)) {
        QByteArray data = updateInfos.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray packages = doc.array();

        int pkgCount = packages.count();
        int appCount = 0;
        bool foundDDEChangelog = false;

        for (QJsonValue val : packages) {
            QJsonObject pack = val.toObject();
            QString packageName = pack["Package"].toString();
            QString metadataDir = "/lastore/metadata/" + packageName;

            if (QFile::exists(metadataDir)) {
                const QString currentVer = pack["CurrentVersion"].toString();
                const QString lastVer = pack["LastVersion"].toString();
                AppUpdateInfo info = getInfo(packageName, currentVer, lastVer);
                if (packageName == "dde" && !info.m_changelog.isEmpty()) {
                    infos.prepend(info);
                    foundDDEChangelog = true;
                } else {
                    infos << info;
                    appCount++;
                }
            }
        }

        if (pkgCount > appCount && !foundDDEChangelog) {
            // If there's no actual package dde update, but there're system patches available,
            // then fake one dde update item.
            AppUpdateInfo ddeUpdateInfo = getInfo("dde", "", "");
            ddeUpdateInfo.m_name = "Deepin";
            ddeUpdateInfo.m_avilableVersion = tr("Patches");
            if(ddeUpdateInfo.m_changelog.isEmpty())
                ddeUpdateInfo.m_changelog = tr("Default Changelog");
            infos.prepend(ddeUpdateInfo);
        }
    }

    return infos;
}

}
}
