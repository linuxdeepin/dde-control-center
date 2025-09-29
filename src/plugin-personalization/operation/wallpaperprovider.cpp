// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QThread>
#include <QScreen>
#include <QImageReader>
#include <QPainter>
#include <QJsonDocument>
#include <QJsonArray>
#include <QImage>
#include <QLoggingCategory>
#include <QUrl>
#include <QDir>
#include <QHash>
#include <QtConcurrent/QtConcurrent>

#include "wallpaperprovider.h"
#include "operation/personalizationdbusproxy.h"
#include "utils.hpp"

Q_LOGGING_CATEGORY(DdcPersonalizationWallpaperWorker, "dcc-personalization-wallpaper-worker")

#define SYS_WALLPAPER_DIR "/usr/share/wallpapers/deepin"
#define SYS_SOLIDE_WALLPAPER_DIR "/usr/share/wallpapers/deepin-solidwallpapers"
#define CUSTOM_SOLIDE_WALLPAPER_DIR "/var/cache/wallpapers/custom-solidwallpapers"
#define CUSTOM_WALLPAPER_DIR "/var/cache/wallpapers/custom-wallpapers"

#define CHECK_RETURN_RUNNING \
    if (Q_UNLIKELY(!m_running.load(std::memory_order_acquire))) \
        return;

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

    connect(m_personalizationProxy, &PersonalizationDBusProxy::WallpaperChanged, this, &WallpaperProvider::onWallpaperChangedFromDaemon);
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
    Q_UNUSED(type)
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
        , thumbnail, del, fileInfo.lastModified().toMSecsSinceEpoch(), false, false});

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
