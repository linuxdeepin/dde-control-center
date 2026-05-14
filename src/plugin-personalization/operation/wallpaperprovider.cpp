// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QThread>
#include <QScreen>
#include <QImageReader>
#include <QPainter>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QImage>
#include <QLoggingCategory>
#include <QUrl>
#include <QDir>
#include <QHash>
#include <QProcess>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QPointer>
#include <QtConcurrent/QtConcurrent>
#include <algorithm>
#include <optional>

#include "wallpaperprovider.h"
#include "operation/model/wallpapermodel.h"
#include "operation/personalizationdbusproxy.h"
#include "utils.hpp"

Q_LOGGING_CATEGORY(DdcPersonalizationWallpaperWorker, "dcc-personalization-wallpaper-worker")

#define SYS_WALLPAPER_DIR "/usr/share/wallpapers/deepin"
#define SYS_SOLIDE_WALLPAPER_DIR "/usr/share/wallpapers/deepin-solidwallpapers"
#define SYS_LIVE_WALLPAPER_DIR "/usr/share/wallpapers/deepin-livewallpapers"
#define CUSTOM_SOLIDE_WALLPAPER_DIR "/var/cache/wallpapers/custom-solidwallpapers"
#define CUSTOM_WALLPAPER_DIR "/var/cache/wallpapers/custom-wallpapers"

// job name: dcc-wallpaper|<WallpaperType>
#define SPLIT_SYMBOL "|"
#define LASTORE_BASE_FLAG "dcc-wallpaper"

#define CHECK_RETURN_RUNNING \
    if (Q_UNLIKELY(!m_running.load(std::memory_order_acquire))) \
        return;

// Solid color sort order: 从浅到深、从暖到冷 (light to dark, warm to cold)
static int solidColorSortOrder(const QString &path)
{
    const QString fileName = QFileInfo(QUrl(path).toLocalFile().isEmpty() ? path : QUrl(path).toLocalFile()).baseName().toLower();
    // Exact filename stem matching, ordered light-to-dark then warm-to-cold
    const QList<QString> order = {
        "mono-orange",
        "mono-dark-red",
        "mono-orange-red",
        "mono-rose-red",
        "mono-light-green",
        "mono-blue",
        "mono-blue-green",
        "mono-dark-green",
        "mono-blue-purple",
        "mono-black"
    };
    int idx = order.indexOf(fileName);
    return (idx >= 0) ? idx : order.size();
}

WallpaperProvider::WallpaperProvider(PersonalizationDBusProxy *PersonalizationDBusProxy, PersonalizationModel *model, QObject *parent) : QObject(parent)
{
    m_workThread = new QThread(this);

    m_personalizationProxy = PersonalizationDBusProxy;
    m_worker = new InterfaceWorker(PersonalizationDBusProxy);
    m_model = model;
    m_worker->moveToThread(m_workThread);
    m_workThread->start();

    connect(m_worker, &InterfaceWorker::pushBackground, this, &WallpaperProvider::setWallpaper, Qt::QueuedConnection);
    connect(m_worker, &InterfaceWorker::pushOneBackground, this, &WallpaperProvider::pushWallpaper, Qt::QueuedConnection);
    connect(m_worker, &InterfaceWorker::listFinished, this, &WallpaperProvider::fetchFinish);
    connect(m_worker, &InterfaceWorker::videoThumbnailReady, this, &WallpaperProvider::onVideoThumbnailReady, Qt::QueuedConnection);

    connect(m_personalizationProxy, &PersonalizationDBusProxy::WallpaperChanged, this, &WallpaperProvider::onWallpaperChangedFromDaemon);
    connect(m_personalizationProxy, &PersonalizationDBusProxy::JobListChanged, this, &WallpaperProvider::onJobListChanged);
}

WallpaperProvider::~WallpaperProvider()
{
    m_worker->terminate();
    m_workThread->quit();
    m_workThread->wait(5000);

    if (m_workThread->isRunning()) {
        m_workThread->terminate();
    }

    delete m_worker;
    m_worker = nullptr;
}

void WallpaperProvider::fetchData(WallpaperType type)
{
    onJobListChanged(m_personalizationProxy->jobList());
    QMetaObject::invokeMethod(m_worker, "startListBackground", Qt::QueuedConnection, Q_ARG(WallpaperType, type));
}

bool WallpaperProvider::isColor(const QString &path)
{
    // these dirs save solid color wallpapers.
    return path.startsWith(CUSTOM_SOLIDE_WALLPAPER_DIR)
            || path.startsWith(SYS_SOLIDE_WALLPAPER_DIR);
}

void WallpaperProvider::setWallpaper(const QList<WallpaperItemPtr> &items, WallpaperType type)
{
    qCDebug(DdcPersonalizationWallpaperWorker) << "get wallpaper list" << items.size() << "type:" << type;

    switch (type) {
        case WallpaperType::Wallpaper_Sys:
            m_wallpaperList[WallpaperType::Wallpaper_Sys] = items;
            m_model->getSysWallpaperModel()->resetData(items);
            break;
        case WallpaperType::Wallpaper_Custom:
            m_wallpaperList[WallpaperType::Wallpaper_Custom] = items;
            m_model->getCustomWallpaperModel()->resetData(items);
            break;
        case WallpaperType::Wallpaper_Solid:
            m_wallpaperList[WallpaperType::Wallpaper_Solid] = items;
            m_model->getSolidWallpaperModel()->resetData(items);
            break;
        case WallpaperType::Wallpaper_Live:
            m_wallpaperList[WallpaperType::Wallpaper_Live] = items;
            m_model->getLiveWallpaperModel()->resetData(items);
            break;
        default:
            break;
    }
}

void WallpaperProvider::pushWallpaper(WallpaperItemPtr item, WallpaperType type)
{
    qCDebug(DdcPersonalizationWallpaperWorker) << "push wallpaper" << item->url<< "type:" << type;

    switch (type) {
        case WallpaperType::Wallpaper_Sys:
            m_wallpaperList[WallpaperType::Wallpaper_Sys].append(item);
            m_model->getSysWallpaperModel()->appendItem(item);
            break;
        case WallpaperType::Wallpaper_Custom:
            m_wallpaperList[WallpaperType::Wallpaper_Custom].append(item);
            m_model->getCustomWallpaperModel()->appendItem(item);
            break;
        case WallpaperType::Wallpaper_Solid:
            m_wallpaperList[WallpaperType::Wallpaper_Solid].append(item);
            m_model->getSolidWallpaperModel()->appendItem(item);
            break;
        case WallpaperType::Wallpaper_Live:
            m_wallpaperList[WallpaperType::Wallpaper_Live].append(item);
            m_model->getLiveWallpaperModel()->appendItem(item);
            break;
        default:
            break;
    }
    emit fetchFinish();
}

WallpaperType WallpaperProvider::getWallpaperType(const QString &path)
{
    for(auto iter = m_wallpaperList.begin(); iter != m_wallpaperList.end(); ++iter) {
        for (const auto &item : iter.value()) {
            if (item->url == path) {
                return iter.key();
            }
        }
    }

    if (path.startsWith(SYS_WALLPAPER_DIR)) {
        return WallpaperType::Wallpaper_Sys;
    } else if (path.startsWith(CUSTOM_WALLPAPER_DIR)) {
        return WallpaperType::Wallpaper_Custom;
    } else if (path.startsWith(CUSTOM_SOLIDE_WALLPAPER_DIR)) {
        return WallpaperType::Wallpaper_Solid;
    } else if (path.startsWith(SYS_SOLIDE_WALLPAPER_DIR)) {
        return WallpaperType::Wallpaper_Solid;
    }

    return WallpaperType::Wallpaper_Unknown;
}

void WallpaperProvider::removeWallpaper(const QString &url)
{
    qCDebug(DdcPersonalizationWallpaperWorker) << "remove wallpaper" << url;

    auto type = getWallpaperType(url);

    WallpaperItemPtr removeItem = nullptr;
    for(auto iter = m_wallpaperList.begin(); iter!= m_wallpaperList.end(); ++iter) {
        for (const auto &item : iter.value()) {
            if (item->url == url) {
                removeItem = item;
                break;
            }
        }
    }

    if (!removeItem) {
        return;
    }

    switch (type) {
        case WallpaperType::Wallpaper_Sys:
            m_model->getSysWallpaperModel()->removeItem(removeItem);
            m_wallpaperList[WallpaperType::Wallpaper_Sys].removeAll(removeItem);
            break;
        case WallpaperType::Wallpaper_Custom:
            m_model->getCustomWallpaperModel()->removeItem(removeItem);
            m_wallpaperList[WallpaperType::Wallpaper_Custom].removeAll(removeItem);
            break;
        case WallpaperType::Wallpaper_Solid:
            m_model->getSolidWallpaperModel()->removeItem(removeItem);
            m_wallpaperList[WallpaperType::Wallpaper_Solid].removeAll(removeItem);
            break;
        default:
            break;
    }
}

void WallpaperProvider::addWallpaper(const QString &url)
{
    qCDebug(DdcPersonalizationWallpaperWorker) << "add wallpaper" << url;

    const auto path = deCodeURI(url);
    WallpaperType type = getWallpaperType(path);
    if (type == WallpaperType::Wallpaper_Unknown) {
        qCWarning(DdcPersonalizationWallpaperWorker) << "add wallpaper type unknown" << path;
        return;
    }
    QMetaObject::invokeMethod(m_worker, "startListOne", Qt::QueuedConnection, Q_ARG(QString, path), Q_ARG(WallpaperType, type));
}

void WallpaperProvider::onWallpaperChangedFromDaemon(const QString &user, uint mode, const QStringList &paths)
{
    if (user != currentUserName()) {
        return;
    }

    if (mode == 0) { // remove wallpaper
        for (const auto &path : paths) {
            removeWallpaper(enCodeURI(path, "file://"));
        }
    } else if (mode == 1) { // add wallpaper
        for (const auto &path : paths) {
            addWallpaper(enCodeURI(path, "file://"));
        }
    }
}

WallpaperItemPtr InterfaceWorker::createItem(const QString &path, bool del, WallpaperType type)
{
    if (path.isEmpty())
        return {};

    QUrl url(path);

    QFileInfo fileInfo;

    if (isURI(path)) {
        fileInfo = QFileInfo(url.toLocalFile());
    } else {
        url = QUrl::fromLocalFile(path);
        fileInfo = QFileInfo(path);
    }
    const QString &thumbnail = url.toString();
    
    auto ptr =  WallpaperItemPtr(new WallpaperItem{url.toString(), url.toString()
        , thumbnail, del, fileInfo.lastModified().toMSecsSinceEpoch(), false, false
        , "", Download_Installed, 1, type});

    return ptr;
}

InterfaceWorker::InterfaceWorker(PersonalizationDBusProxy *proxy, QObject *parent)
    : QObject(parent)
    , m_proxy(proxy)
    , m_running(true)
{

}

void InterfaceWorker::terminate()
{
    m_running.store(false, std::memory_order_release);
}

void InterfaceWorker::startListBackground(WallpaperType type)
{
    m_running.store(true, std::memory_order_release);
    CHECK_RETURN_RUNNING
    switch (type) {
        case WallpaperType::Wallpaper_all:
            getCustomBackground();
            getSysBackground();
            getSolodBackground();
            getLiveBackground();
            break;
        case WallpaperType::Wallpaper_Sys:
            getSysBackground();
            break;
        case WallpaperType::Wallpaper_Custom:
            getCustomBackground();
            break;
        case WallpaperType::Wallpaper_Solid:
            getSolodBackground();
            break;
        case WallpaperType::Wallpaper_Live:
            getLiveBackground();
            break;
        default:
            break;
    }
    Q_EMIT listFinished();
}

void InterfaceWorker::startListOne(const QString &path, WallpaperType type)
{
    CHECK_RETURN_RUNNING
    bool canDel = type == WallpaperType::Wallpaper_Custom;
    if (type == WallpaperType::Wallpaper_Solid) {
        canDel = path.startsWith(CUSTOM_SOLIDE_WALLPAPER_DIR);
    }

    WallpaperItemPtr ptr = createItem(path, canDel, type);
    if (ptr) {
        Q_EMIT pushOneBackground(ptr, type);
    }
}

void InterfaceWorker::getSysBackground()
{
    CHECK_RETURN_RUNNING
    QList<WallpaperItemPtr> wallpapers;
    auto list = fetchWallpaper(SYS_WALLPAPER_DIR);
    for (const auto &path : list) {
        CHECK_RETURN_RUNNING
        WallpaperItemPtr ptr = createItem(path, false, WallpaperType::Wallpaper_Sys);
        if (ptr)
            wallpapers.append(ptr);
    }
    Q_EMIT pushBackground(wallpapers, WallpaperType::Wallpaper_Sys);
}

void InterfaceWorker::getCustomBackground()
{
    CHECK_RETURN_RUNNING
    QList<WallpaperItemPtr> wallpapers;
    auto customList = m_proxy->getCustomWallpaper(currentUserName());
    for (const auto &path : customList) {
        CHECK_RETURN_RUNNING
        if (WallpaperProvider::isColor(path)) {
            continue;
        }
        WallpaperItemPtr ptr = createItem(path, true, WallpaperType::Wallpaper_Custom);
        if (ptr)
            wallpapers.append(ptr);
    }
    Q_EMIT pushBackground(wallpapers, WallpaperType::Wallpaper_Custom);
}

void InterfaceWorker::getSolodBackground()
{
    CHECK_RETURN_RUNNING
    QList<WallpaperItemPtr> wallpapers;
    auto customList = m_proxy->getCustomWallpaper(currentUserName());
    for (const auto &path : customList) {
        CHECK_RETURN_RUNNING
        if (!WallpaperProvider::isColor(path)) {
            continue;
        }

        WallpaperItemPtr ptr = createItem(path, true, WallpaperType::Wallpaper_Solid);
        if (ptr)
            wallpapers.append(ptr);
    }
    auto list = fetchWallpaper(SYS_SOLIDE_WALLPAPER_DIR);
    for (const auto &path : list) {
        WallpaperItemPtr ptr = createItem(path, false, WallpaperType::Wallpaper_Solid);
        if (ptr)
            wallpapers.append(ptr);
    }
    // Sort: 从浅到深、从暖到冷 (light to dark, warm to cold)
    std::stable_sort(wallpapers.begin(), wallpapers.end(), [](const WallpaperItemPtr &a, const WallpaperItemPtr &b) {
        return solidColorSortOrder(a->url) < solidColorSortOrder(b->url);
    });
    for (int i = 0; i < wallpapers.size(); ++i) {
        wallpapers[i]->lastModifiedTime = wallpapers.size() - i;
    }
    Q_EMIT pushBackground(wallpapers, WallpaperType::Wallpaper_Solid);
}

QStringList InterfaceWorker::fetchWallpaper(const QString &dir)
{
    QStringList walls;

    QDir qdir(dir);
    if (!qdir.exists())
        return walls;

    QFileInfoList fileInfoList = qdir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
    for (auto file : fileInfoList) {
        QImageReader reader(file.filePath());
        if (!reader.canRead()) {
            continue;
        }
        walls.append(file.filePath());
    }

    return walls;
}

void InterfaceWorker::getLiveBackground()
{
    CHECK_RETURN_RUNNING
    QList<WallpaperItemPtr> wallpapers;

    QDir dir(SYS_LIVE_WALLPAPER_DIR);
    if (!dir.exists()) {
        qCWarning(DdcPersonalizationWallpaperWorker) << "Live wallpaper dir not exists:" << SYS_LIVE_WALLPAPER_DIR;
        Q_EMIT pushBackground(wallpapers, WallpaperType::Wallpaper_Live);
        return;
    }

    QFile metaFile(dir.absoluteFilePath("metadata.json"));
    if (!metaFile.exists()) {
        qCWarning(DdcPersonalizationWallpaperWorker) << "metadata.json not found, fallback to scan mp4 files";
        QStringList nameFilters;
        nameFilters << "*.mp4" << "*.MP4" << "*.mov" << "*.MOV" << "*.avi" << "*.AVI";
        QFileInfoList fileInfoList = dir.entryInfoList(nameFilters, QDir::Files);

        for (const auto &fileInfo : fileInfoList) {
            CHECK_RETURN_RUNNING
            QString videoPath = fileInfo.absoluteFilePath();
            QUrl url = QUrl::fromLocalFile(videoPath);
            QString cacheDir = thumbnailCacheDir();
            QDir().mkpath(cacheDir);
            QString cacheName = QString(QCryptographicHash::hash(videoPath.toUtf8(), QCryptographicHash::Md5).toHex()) + ".png";
            QString cachePath = cacheDir + "/" + cacheName;
            QString thumbnail = QFile::exists(cachePath) ? cachePath : videoPath;

            WallpaperItemPtr ptr(new WallpaperItem{
                url.toString(), url.toString(), QUrl::fromLocalFile(thumbnail).toString(),
                false, fileInfo.lastModified().toMSecsSinceEpoch(), false, false,
                "", Download_Installed, 1, Wallpaper_Live
            });
            if (ptr) {
                if (!QFile::exists(cachePath))
                    generateVideoThumbnail(videoPath, cachePath, url.toString());
                wallpapers.append(ptr);
            }
        }
        Q_EMIT pushBackground(wallpapers, WallpaperType::Wallpaper_Live);
        return;
    }

    if (!metaFile.open(QIODevice::ReadOnly)) {
        qCWarning(DdcPersonalizationWallpaperWorker) << "Failed to open metadata.json";
        Q_EMIT pushBackground(wallpapers, WallpaperType::Wallpaper_Live);
        return;
    }

    QJsonParseError parseErr;
    QJsonDocument doc = QJsonDocument::fromJson(metaFile.readAll(), &parseErr);
    metaFile.close();
    if (parseErr.error != QJsonParseError::NoError || !doc.isArray()) {
        qCWarning(DdcPersonalizationWallpaperWorker) << "metadata.json parse error:" << parseErr.errorString();
        Q_EMIT pushBackground(wallpapers, WallpaperType::Wallpaper_Live);
        return;
    }

    QString cacheDir = thumbnailCacheDir();
    QDir().mkpath(cacheDir);

    QJsonArray entries = doc.array();
    for (const auto &entry : entries) {
        CHECK_RETURN_RUNNING
        QJsonObject obj = entry.toObject();
        QString id = obj.value("id").toString();
        QString thumbnailRel = obj.value("thumbnail").toString();
        QString pathRel = obj.value("path").toString();
        QString packageName = obj.value("packageName").toString();

        if (id.isEmpty() || pathRel.isEmpty())
            continue;

        QString videoAbsPath = dir.absoluteFilePath(pathRel);
        QUrl url = QUrl::fromLocalFile(videoAbsPath);
        bool videoExists = QFile::exists(videoAbsPath);

        QString thumbnailPath;
        bool needGenerate = false;
        if (!thumbnailRel.isEmpty()) {
            QString thumbAbsPath = dir.absoluteFilePath(thumbnailRel);
            if (QFile::exists(thumbAbsPath)) {
                thumbnailPath = thumbAbsPath;
            } else {
                needGenerate = true;
            }
        } else {
            needGenerate = true;
        }

        if (needGenerate && videoExists) {
            QString cacheName = QString(QCryptographicHash::hash(videoAbsPath.toUtf8(), QCryptographicHash::Md5).toHex()) + ".png";
            QString cachePath = cacheDir + "/" + cacheName;
            if (QFile::exists(cachePath)) {
                thumbnailPath = cachePath;
                needGenerate = false;
            } else {
                thumbnailPath = cachePath;
            }
        }

        WallpaperInstallStatus status = videoExists ? Download_Installed : Download_NotInstalled;

        WallpaperItemPtr ptr(new WallpaperItem{
            url.toString(),
            url.toString(),
            thumbnailPath.isEmpty() ? url.toString() : QUrl::fromLocalFile(thumbnailPath).toString(),
            false, 0, false, false,
            packageName, status, videoExists ? 1.0 : 0, Wallpaper_Live
        });

        if (ptr) {
            wallpapers.append(ptr);
            if (needGenerate && videoExists)
                generateVideoThumbnail(videoAbsPath, thumbnailPath, id);
        }
    }

    qCDebug(DdcPersonalizationWallpaperWorker) << "Found" << wallpapers.size() << "live wallpapers from metadata";
    Q_EMIT pushBackground(wallpapers, WallpaperType::Wallpaper_Live);
}

QString InterfaceWorker::thumbnailCacheDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/live-wallpaper-thumbnails";
}

void InterfaceWorker::generateVideoThumbnail(const QString &videoPath, const QString &cachePath, const QString &id)
{
    QPointer<InterfaceWorker> guard(this);
    (void)QtConcurrent::run([guard, videoPath, cachePath, id]() {
        if (!guard)
            return;

        if (!guard->m_running.load(std::memory_order_acquire))
            return;

        QProcess ffmpeg;
        ffmpeg.setProgram("/usr/bin/ffmpeg");
        ffmpeg.setArguments({"-i", videoPath, "-ss", "00:00:01", "-vframes", "1",
                             "-vf", "scale=480:-1", "-update", "1", "-y", cachePath});
        ffmpeg.start();
        ffmpeg.waitForFinished(5000);

        if (!guard)
            return;

        if (ffmpeg.exitCode() == 0 && QFile::exists(cachePath)) {
            Q_EMIT guard->videoThumbnailReady(id, cachePath);
        }
    });
}

void WallpaperProvider::installWallpaper(const QString &itemId, WallpaperType type)
{
    qCWarning(DdcPersonalizationWallpaperWorker) << "download live wallpaper requested:" << itemId << type;

    auto wallpaper = findWallpaperItem(itemId, type);
    if (wallpaper.has_value() && !wallpaper.value()->packageName.isEmpty()) {
        wallpaper.value()->installStatus = Download_Installing;
        wallpaper.value()->installProgress = 0;
        auto model = getModel(type);
        if (!model) {
            qCWarning(DdcPersonalizationWallpaperWorker()) << "get model error";
            return;
        }
        model->updateItemData(wallpaper.value(), {
            Item_InstallStatus_Role, Item_InstallProgress_Role
        });

        const QString jobName = QString(LASTORE_BASE_FLAG) + SPLIT_SYMBOL + QString::number(static_cast<int>(type));

        auto call = m_personalizationProxy->InstallPackage(jobName, wallpaper.value()->packageName);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, call, wallpaper, watcher, model] {
            if (call.isError()) {
                wallpaper.value()->installStatus = WallpaperEnums::Download_NotInstalled;
                wallpaper.value()->installProgress = 0;
                model->updateItemData(wallpaper.value(), {
                    Item_InstallStatus_Role, Item_InstallProgress_Role
                });
                qCWarning(DdcPersonalizationWallpaperWorker) << "install job create error: " << call.error();
            } else {
                setWantToSetWallpaper(wallpaper.value());
                emit wantToSetWallpaperStatusChanged(m_wantToSetWallpaper->installStatus);
                emit wantToSetWallpaperProgressChanged(m_wantToSetWallpaper->installProgress);
            }
            watcher->deleteLater();
        });
    }
}

void WallpaperProvider::onVideoThumbnailReady(const QString &id, const QString &thumbnailPath)
{
    auto wallpaper = findWallpaperItem(id, Wallpaper_Live);
    if (wallpaper.has_value()) {
        qCDebug(DdcPersonalizationWallpaperWorker) << "thumbnail ready for" << id << thumbnailPath;
        wallpaper.value()->thumbnail = QUrl::fromLocalFile(thumbnailPath).toString();
        m_model->getLiveWallpaperModel()->updateItemData(wallpaper.value(), {Item_Thumbnail_Role});
    }
}

void WallpaperProvider::onJobListChanged(const QList<QDBusObjectPath> &value)
{
    QList<QString> ClearList = m_jobInterMap.keys();
    auto findWallpaper = [this](JobInter &job) -> std::optional<WallpaperItemPtr>{
        bool ok = false;
        auto type = job.name().split(SPLIT_SYMBOL).value(1).toInt(&ok);
        if (ok) {
            for (auto wallpaperItem : m_wallpaperList.value(static_cast<WallpaperType>(type))) {
                if (wallpaperItem->packageName == job.packages().value(0)) {
                    return wallpaperItem;
                }
            }
        }

        return std::nullopt;
    };

    for (const auto &job : value) {
        const QString &jobPath = job.path();
        JobInter jobInter(jobPath, this);
        if (!jobInter.isValid()) {
            qCWarning(DdcPersonalizationWallpaperWorker) << "Job is invalid";
            continue;   
        }

        // id maybe scrapped
        const QString &id = jobInter.id();
        qCInfo(DdcPersonalizationWallpaperWorker) << "Job id : " << id;
        static const QRegularExpression installRe("^\\d*install$");
        if (installRe.match(id).hasMatch() && jobInter.name().startsWith("dcc-wallpaper")) {
            if (jobInter.status() == "failed") {
                m_personalizationProxy->CleanJob(jobInter.id());
            }
            if (m_jobInterMap.contains(jobPath)) {
                ClearList.removeAll(jobPath);
            } else {
                if(auto wallpaper = findWallpaper(jobInter); wallpaper.has_value()) {
                    createInstallJob(jobPath, wallpaper.value());
                }
            }
        }
    }

    for (const auto &job : ClearList) {
        qCInfo(DdcPersonalizationWallpaperWorker()) << "changed delete job" << job;
        m_jobInterMap.value(job)->deleteLater();
        m_jobInterMap.remove(job);
    }
}

void WallpaperProvider::createInstallJob(const QString &jobPath, WallpaperItemPtr wallpaperItem)
{
    qCInfo(DdcPersonalizationWallpaperWorker) << "create job" << jobPath;
    if (jobPath.isEmpty()) {
        return;
    }

    auto job = new JobInter(jobPath, this);
    auto value = job->progress();
    auto status = job->status();
    auto model = getModel(wallpaperItem->type);
    if (!model) {
        qCWarning(DdcPersonalizationWallpaperWorker()) << "can not find model: " << wallpaperItem->url;
        return;
    }
    m_jobInterMap.insert(jobPath, job);
    wallpaperItem->installProgress = value;
    if (status == "succeed" || status == "end") {
        wallpaperItem->installStatus = WallpaperEnums::Download_Installed;
    } else {
        wallpaperItem->installStatus = WallpaperEnums::Download_Installing;
    }

    model->updateItemData(wallpaperItem, {Item_InstallProgress_Role, Item_InstallStatus_Role});

    connect(job, &JobInter::ProgressChanged, this, [wallpaperItem, this, model](double value){
        wallpaperItem->installProgress = value;
        model->updateItemData(wallpaperItem, {Item_InstallProgress_Role});
        if (m_wantToSetWallpaper == wallpaperItem) {
            emit wantToSetWallpaperProgressChanged(wallpaperItem->installProgress);
        }
    });

    connect(job, &JobInter::StatusChanged, this, [job, wallpaperItem, this](QString status){
        auto model = getModel(wallpaperItem->type);
        if (!model) {
            qCWarning(DdcPersonalizationWallpaperWorker()) << "get model error";
            return;
        }
        if (status == "succeed") {
            wallpaperItem->installStatus = WallpaperEnums::Download_Installed;
        } else if (status == "failed"){
            wallpaperItem->installStatus = WallpaperEnums::Download_NotInstalled;
            m_personalizationProxy->CleanJob(job->id());
            qCWarning(DdcPersonalizationWallpaperWorker()) << "download wallpaper error, wallpaper: " << job->packages() << job->description();
        } else if (status != "end"){
            wallpaperItem->installStatus = WallpaperEnums::Download_Installing;
        }

        if (m_wantToSetWallpaper == wallpaperItem) {
            emit wantToSetWallpaperStatusChanged(wallpaperItem->installStatus);
        }

        model->updateItemData(wallpaperItem, {Item_InstallStatus_Role});
    });
}

void WallpaperProvider::setWantToSetWallpaper(WallpaperItemPtr wallpaper)
{
    if (wallpaper != m_wantToSetWallpaper) {
        m_wantToSetWallpaper = wallpaper;
        emit m_model->wantToSetWallpaperChanged();
        emit wantToSetWallpaperChanged(wallpaper);
    }
}

std::optional<WallpaperItemPtr> WallpaperProvider::findWallpaperItem(const QString &url, WallpaperType type)
{
    const auto it = m_wallpaperList.find(type);
    if (it == m_wallpaperList.end()) {
        return std::nullopt;
    }

    for (const auto &wallpaperItem : it.value()) {
        if (wallpaperItem && wallpaperItem->url == url) {
            return wallpaperItem;
        }
    }

    return std::nullopt;
}

WallpaperModel *WallpaperProvider::getModel(WallpaperType type) const
{
    switch (type) {
        case WallpaperEnums::Wallpaper_Sys:
            return m_model->getSysWallpaperModel();
        case WallpaperEnums::Wallpaper_Solid:
            return m_model->getSolidWallpaperModel();
        case WallpaperEnums::Wallpaper_Live:
            return m_model->getLiveWallpaperModel();
        case WallpaperEnums::Wallpaper_Custom:
            return m_model->getCustomWallpaperModel();
        default:
            return nullptr;
    }
}
