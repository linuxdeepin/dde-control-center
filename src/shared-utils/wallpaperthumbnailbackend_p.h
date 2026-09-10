// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>
#include <QtPlugin>

class WallpaperThumbnailBackend
{
public:
    virtual ~WallpaperThumbnailBackend() = default;

    virtual bool generate(const QString &videoPath, const QString &outputPath) const = 0;
};

#define WallpaperThumbnailBackendInterface_iid "org.deepin.dde.control-center.WallpaperThumbnailBackend/1.0"
Q_DECLARE_INTERFACE(WallpaperThumbnailBackend, WallpaperThumbnailBackendInterface_iid)
