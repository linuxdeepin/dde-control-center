// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "WallpaperManager.h"

#include "WayQtLogging.h"

#include <QDebug>
#include <QLoggingCategory>

// ── WQt::Wallpaper ───────────────────────────────────────────────────

WQt::Wallpaper::Wallpaper(struct ::treeland_wallpaper_v1 *object, wl_output *output, QObject *parent)
    : QObject(parent)
    , QtWayland::treeland_wallpaper_v1(object)
    , mOutput(output)
{
}

WQt::Wallpaper::~Wallpaper()
{
    if (isInitialized())
        QtWayland::treeland_wallpaper_v1::destroy();
}

bool WQt::Wallpaper::isValid() const
{
    return isInitialized();
}

wl_output *WQt::Wallpaper::output() const
{
    return mOutput;
}

QString WQt::Wallpaper::fileSource() const
{
    return mFileSource;
}

uint32_t WQt::Wallpaper::sourceType() const
{
    return mSourceType;
}

void WQt::Wallpaper::treeland_wallpaper_v1_changed(uint32_t role, uint32_t sourceType, const QString &fileSource)
{
    qCInfo(DccWayQt) << "Wallpaper changed:" << mOutput << "role:" << role << "sourceType:" << sourceType << fileSource;
    mSourceType = sourceType;
    mFileSource = fileSource;
    Q_EMIT changed(mFileSource, mSourceType, role);
}

void WQt::Wallpaper::treeland_wallpaper_v1_failed(const QString &source, uint32_t error)
{
    qCWarning(DccWayQt) << "Wallpaper failed:" << source << "error:" << error;
    Q_EMIT failed(source, error);
}

// ── WQt::WallpaperManager ────────────────────────────────────────────

WQt::WallpaperManager::WallpaperManager(QObject *parent)
    : QWaylandClientExtensionTemplate<WQt::WallpaperManager>(1)
{
    setParent(parent);
}

WQt::WallpaperManager::~WallpaperManager()
{
    for (auto *wp : mWallpapers) {
        disconnect(wp, &QObject::destroyed, this, nullptr);
    }
    qDeleteAll(mWallpapers);
}

WQt::Wallpaper *WQt::WallpaperManager::getWallpaper(wl_output *output)
{
    if (!output) {
        return nullptr;
    }

    void *key = reinterpret_cast<void *>(output);
    if (mWallpapers.contains(key)) {
        return mWallpapers.value(key);
    }

    auto *obj = QtWayland::treeland_wallpaper_manager_v1::get_treeland_wallpaper(output, nullptr);
    if (!obj) {
        return nullptr;
    }

    auto *wallpaper = new WQt::Wallpaper(obj, output, this);
    connect(wallpaper, &QObject::destroyed, this, [this, key]() {
        mWallpapers.remove(key);
    });
    mWallpapers.insert(key, wallpaper);
    return wallpaper;
}

void WQt::WallpaperManager::removeWallpaper(wl_output *output)
{
    if (!output) {
        return;
    }

    void *key = reinterpret_cast<void *>(output);
    auto *wallpaper = mWallpapers.take(key);
    if (!wallpaper) {
        return;
    }

    disconnect(wallpaper, &QObject::destroyed, this, nullptr);
    wallpaper->deleteLater();
}
