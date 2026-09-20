// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "wallpapersync.h"

#include <QCoreApplication>
#include <QPointer>
#include <QThread>

namespace {
QPointer<WallpaperSync> wallpaperSyncInstance;
} // namespace

WallpaperSync *WallpaperSync::instance()
{
    if (wallpaperSyncInstance)
        return wallpaperSyncInstance;

    auto *application = QCoreApplication::instance();
    if (!application)
        return nullptr;

    // The lambda is defined inside this static member function, so it inherits
    // WallpaperSync's access rights and may invoke the private constructor.
    auto create = [application]() {
        if (!wallpaperSyncInstance)
            wallpaperSyncInstance = new WallpaperSync(application);
    };

    if (QThread::currentThread() == application->thread()) {
        create();
    } else {
        QMetaObject::invokeMethod(application, create, Qt::BlockingQueuedConnection);
    }

    return wallpaperSyncInstance;
}

QVariantMap WallpaperSync::wallpaperMap() const
{
    return m_wallpaperMap;
}

void WallpaperSync::setWallpaper(const QString &monitorName, const QString &source, uint sourceType)
{
    const QVariantMap wallpaper{ { QStringLiteral("source"), source },
                                 { QStringLiteral("sourceType"), sourceType } };

    if (m_wallpaperMap.value(monitorName) == wallpaper)
        return;

    m_wallpaperMap.insert(monitorName, wallpaper);
    Q_EMIT wallpaperChanged(monitorName, source, sourceType);
}

WallpaperSync::WallpaperSync(QObject *parent)
    : QObject(parent)
{
}
