#include "updatework.h"

UpdateWork::UpdateWork(UpdateModel* model, QObject *parent)
    : QObject(parent),
      m_model(model),
      m_downloadJob(nullptr),
      m_checkUpdateJob(nullptr)
{
    m_updateInter = new UpdateInter("com.deepin.lastore",
                                    "/com/deepin/lastore",
                                    QDBusConnection::systemBus(), this);

    m_managerInter = new ManagerInter("com.deepin.lastore",
                                      "/com/deepin/lastore",
                                      QDBusConnection::systemBus(), this);

    connect(this, SIGNAL(mirrorSourceChanged(QString)), m_model, SLOT(setDefaultMirror(QString)));
    connect(this,SIGNAL(autoCheckUpdatesChanged(bool)), m_model, SLOT(setAutoUpdate(bool)));
    connect(m_updateInter, SIGNAL(MirrorSourceChanged(QString)), this, SIGNAL(mirrorSourceChanged(QString)));
    connect(m_updateInter, SIGNAL(AutoCheckUpdatesChanged(bool)), this, SIGNAL(autoCheckUpdatesChanged(bool)));
    connect(this, &UpdateWork::appInfos, m_model, &UpdateModel::appInfos);
    connect(this, &UpdateWork::packageDownloadSize, m_model, &UpdateModel::packageDownloadSize);
    connect(this, &UpdateWork::progressChanged, m_model, &UpdateModel::progressChanged);
    connect(this, &UpdateWork::statusChanged, m_model, &UpdateModel::statusChanged);

    m_model->setMirrorInfoList(mirrorInfos());
    m_model->setDefaultMirror(defaultMirror());
    m_model->setAutoUpdate(autoUpdate());
}

MirrorInfoList UpdateWork::mirrorInfos() const
{
    return m_updateInter->ListMirrorSources(QLocale().name()).value();
}

QString UpdateWork::defaultMirror() const
{
    return m_updateInter->mirrorSource();
}

void UpdateWork::setMirrorSource(const QString &src)
{
    m_updateInter->SetMirrorSource(src);
}

bool UpdateWork::autoUpdate() const
{
    return m_updateInter->autoCheckUpdates();
}

void UpdateWork::setAutoUpdate(bool autoUpdate)
{
    m_updateInter->SetAutoCheckUpdates(autoUpdate);
}

void UpdateWork::prepareDistUpgrade()
{
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_managerInter->PrepareDistUpgrade(), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher] {
        const QDBusPendingReply<QDBusObjectPath> &reply = *watcher;
        const QDBusObjectPath &path = reply;
        qDebug() << "start download job: " << path.path() << reply.error();

        JobInter *job = new JobInter("com.deepin.lastore",
                                  path.path(),
                                  QDBusConnection::systemBus(), this);
        if(job->isValid())
        {
            loadDownloadJob(job);
        }
        else
        {
            job->deleteLater();
        }
        watcher->deleteLater();
    });
}

bool UpdateWork::pauseJob()
{
    if(m_downloadJob)
    {
        m_managerInter->PauseJob(m_downloadJob->id());
        return true;
    }

    return false;
}

bool UpdateWork::startJob()
{
    if(m_downloadJob)
    {
        m_managerInter->StartJob(m_downloadJob->id());
        return true;
    }

    return false;
}

void UpdateWork::onStatus(const QString &status)
{
    Q_UNUSED(status);

    if(!m_downloadJob || !m_downloadJob->isValid())
        return;

    QString st = m_downloadJob->status();
    if (status == "succeed" || status == "end" || status.isEmpty())
    {
        PowerInter inter("com.deepin.daemon.Power", "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this);
        if(!inter.batteryIsPresent().count() == 0)
        {
            BatteryPercentageInfo batterInfo = inter.batteryPercentage();
            double percent = batterInfo.values().at(0);
            emit statusChanged(true, percent);
        }
        else
        {
            emit statusChanged(false, -1);
        }
    }
}

void UpdateWork::checkUpdate()
{

    QDBusPendingReply<QDBusObjectPath> reply = m_managerInter->UpdateSource();
    reply.waitForFinished();

    const QString jobPath = reply.value().path();
    m_checkUpdateJob = new JobInter("com.deepin.lastore", jobPath, QDBusConnection::systemBus(), this);

    connect(m_checkUpdateJob, SIGNAL(StatusChanged(QString)), this, SLOT(onCheckUpdateStatus(QString)));
}

void UpdateWork::onCheckUpdateStatus(const QString &status)
{
    if(!m_checkUpdateJob)
        return;

    if(status == "end" || status == "failed")
    {
        loadAppList();
        m_checkUpdateJob->deleteLater();
        m_checkUpdateJob = nullptr;

        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_managerInter->PackagesDownloadSize(m_updatablePackages), this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, &UpdateWork::onPackagesDownloadSize);
    }
}

void UpdateWork::onPackagesDownloadSize(QDBusPendingCallWatcher *watcher)
{
    qlonglong size = watcher->reply().arguments().first().toLongLong();

    emit packageDownloadSize(size, m_updatableApps.count());
    watcher->deleteLater();
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
            ddeUpdateInfo.m_changelog = tr("Default Changelog");
            infos.prepend(ddeUpdateInfo);
        }
    }

    return infos;
}

void UpdateWork::loadAppList()
{
    QList<AppUpdateInfo> infos = getInfoList();

    emit appInfos(infos);
    m_updatableApps = updatableApps();
    m_updatablePackages = updatablePackages();
}

QStringList UpdateWork::updatableApps() const
{
    QStringList apps = m_updateInter->updatableApps();
    apps.removeAll("dde");

    return apps;
}

QStringList UpdateWork::updatablePackages() const
{
    QStringList pkgs = m_updateInter->updatablePackages();
    return pkgs;
}

void UpdateWork::loadDownloadJob(JobInter *newJob)
{
    if(m_downloadJob)
        m_downloadJob->deleteLater();

    m_downloadJob = newJob;
    QString status = m_downloadJob->status();

    if (status == "succeed" || status == "end" || status.isEmpty())
    {
        //  finish download
        emit statusChanged(true, 100);
        return;
    }

    connect(m_downloadJob, SIGNAL(ProgressChanged(double)), this, SIGNAL(progressChanged(double)));
    connect(m_downloadJob, SIGNAL(StatusChanged(QString)), this, SLOT(onStatus(QString)));
}
