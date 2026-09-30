// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QVariantMap>

class WallpaperSync : public QObject
{
    Q_OBJECT
public:
    static constexpr uint ImageSourceType = 0;
    static constexpr uint VideoSourceType = 1;

    static WallpaperSync *instance();

    QVariantMap wallpaperMap() const;

public Q_SLOTS:
    void setWallpaper(const QString &monitorName, const QString &source, uint sourceType);

Q_SIGNALS:
    void wallpaperChanged(const QString &monitorName, const QString &source, uint sourceType);

private:
    explicit WallpaperSync(QObject *parent = nullptr);

    QVariantMap m_wallpaperMap;
};
