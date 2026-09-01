// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "qwayland-treeland-wallpaper-manager-unstable-v1.h"

#include <wayland-client.h>

#include <QMap>
#include <QObject>
#include <QtWaylandClient/QWaylandClientExtension>

namespace WQt {
class Wallpaper;
class WallpaperManager;
} // namespace WQt

class WQt::Wallpaper : public QObject, public QtWayland::treeland_wallpaper_v1
{
    Q_OBJECT

    friend class WallpaperManager;

private:
    explicit Wallpaper(struct ::treeland_wallpaper_v1 *object, wl_output *output, QObject *parent = nullptr);

public:
    ~Wallpaper() override;

    bool isValid() const;

    wl_output *output() const;
    QString fileSource() const;
    uint32_t sourceType() const;

protected:
    void treeland_wallpaper_v1_changed(uint32_t role, uint32_t sourceType, const QString &fileSource) override;
    void treeland_wallpaper_v1_failed(const QString &source, uint32_t error) override;

private:
    wl_output *mOutput = nullptr;
    QString mFileSource;
    uint32_t mSourceType = 0;

Q_SIGNALS:
    void changed(const QString &fileSource, uint32_t sourceType, uint32_t role);
    void failed(const QString &source, uint32_t error);
};

class WQt::WallpaperManager : public QWaylandClientExtensionTemplate<WQt::WallpaperManager>, public QtWayland::treeland_wallpaper_manager_v1
{
    Q_OBJECT

public:
    explicit WallpaperManager(QObject *parent = nullptr);
    ~WallpaperManager() override;

    WQt::Wallpaper *getWallpaper(wl_output *output);
    void removeWallpaper(wl_output *output);

private:
    QMap<void *, WQt::Wallpaper *> mWallpapers;
};
