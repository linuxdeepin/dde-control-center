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

#include "wallpaperprovider.h"
#include "operation/personalizationdbusproxy.h"
#include "utils.hpp"

Q_LOGGING_CATEGORY(DdcPersonalizationWallpaperWorker, "dcc-personalization-wallpaper-worker")

#define SYS_WALLPAPER_DIR "/usr/share/wallpapers/deepin"
#define SYS_SOLIDE_WALLPAPER_DIR "/usr/share/wallpapers/deepin-solidwallpapers"
#define CUSTOM_SOLIDE_WALLPAPER_DIR "/var/cache/wallpapers/custom-solidwallpapers"
#define CUSTOM_WALLPAPER_DIR "/var/cache/wallpapers/custom-wallpapers"

WallpaperProvider::WallpaperProvider(PersonalizationDBusProxy *PersonalizationDBusProxy, PersonalizationModel *model, QObject *parent) : QObject(parent)
{
    m_workThread = new QThread(this);

    m_personalizationProxy = PersonalizationDBusProxy;
    m_worker = new InterfaceWorker(PersonalizationDBusProxy);
    m_model = model;
    m_worker->moveToThread(m_workThread);
    m_workThread->start();

    connect(m_worker, &InterfaceWorker::pushBackground, this, &WallpaperProvider::setWallpaper, Qt::QueuedConnection);
}

WallpaperProvider::~WallpaperProvider()
{
    m_worker->terminate();
    m_workThread->quit();
    m_workThread->wait(1000);

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
            m_model->getSysWallpaperModel()->resetData(items);
            break;
        case WallpaperType::Wallpaper_Custom:
            m_model->getCustomWallpaperModel()->resetData(items);
            break;
        case WallpaperType::Wallpaper_Solid:
            m_model->getSolidWallpaperModel()->resetData(items);
            break;
        default:
            break;
    }
}

WallpaperItemPtr WallpaperProvider::createItem(const QString &path, bool del)
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
    return WallpaperItemPtr(new WallpaperItem{url.toString(), url.toString()
        , generateThumbnail(fileInfo.filePath(), QSize(THUMBNAIL_ICON_WIDTH, THUMBNAIL_ICON_HEIGHT)), 
            del, fileInfo.lastModified().toMSecsSinceEpoch()});
}

InterfaceWorker::InterfaceWorker(PersonalizationDBusProxy *proxy, QObject *parent)
    : QObject(parent)
    , m_proxy(proxy)
{

}

void InterfaceWorker::terminate()
{
    m_running = false;
}

void InterfaceWorker::startListBackground(WallpaperType type)
{
    m_running = true;

    switch (type) {
        case WallpaperType::Wallpaper_all:
            getSysBackground();
            getCustomBackground();
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
    }

    m_running = false;
}

void InterfaceWorker::getSysBackground()
{
    QList<WallpaperItemPtr> wallpapers;
    auto list = fetchWallpaper(SYS_WALLPAPER_DIR);
    for (const auto &path : list) {
        WallpaperItemPtr ptr = WallpaperProvider::createItem(path, false);
        if (ptr)
            wallpapers.append(ptr);
    }
    Q_EMIT pushBackground(wallpapers, WallpaperType::Wallpaper_Sys);
}

void InterfaceWorker::getCustomBackground()
{
    QList<WallpaperItemPtr> wallpapers;
    auto customList = m_proxy->getCustomWallpaper(currentUserName());
    for (const auto &path : customList) {
        if (WallpaperProvider::isColor(path)) {
            continue;
        }

        WallpaperItemPtr ptr = WallpaperProvider::createItem(path, true);
        if (ptr)
            wallpapers.append(ptr);
    }
    Q_EMIT pushBackground(wallpapers, WallpaperType::Wallpaper_Custom);
}

void InterfaceWorker::getSolodBackground()
{
    QList<WallpaperItemPtr> wallpapers;
    auto customList = m_proxy->getCustomWallpaper(currentUserName());
    for (const auto &path : customList) {
        if (!WallpaperProvider::isColor(path)) {
            continue;
        }

        WallpaperItemPtr ptr = WallpaperProvider::createItem(path, true);
        if (ptr)
            wallpapers.append(ptr);
    }
    auto list = fetchWallpaper(SYS_SOLIDE_WALLPAPER_DIR);
    for (const auto &path : list) {
        WallpaperItemPtr ptr = WallpaperProvider::createItem(path, false);
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
