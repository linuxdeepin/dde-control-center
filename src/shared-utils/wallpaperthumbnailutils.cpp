// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "wallpaperthumbnailutils.h"
#include "dccbenchmark.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPluginLoader>
#include <QStandardPaths>
#include <QStringList>
#include <QUuid>

#include "wallpaperthumbnailbackend_p.h"

namespace {

QStringList thumbnailBackendPaths()
{
    const QString backendFileName = QStringLiteral(DCC_WALLPAPER_THUMBNAILER_FILENAME);
    return {
        QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(QStringLiteral("../lib/") + backendFileName),
        QStringLiteral(DCC_WALLPAPER_THUMBNAILER_INSTALL_PATH),
        backendFileName,
    };
}

WallpaperThumbnailBackend *loadThumbnailBackend()
{
    QPluginLoader loader;
    loader.setLoadHints(QLibrary::PreventUnloadHint);

    QStringList errors;
    for (const QString &path : thumbnailBackendPaths()) {
        loader.setFileName(path);
        QObject *instance = loader.instance();
        if (!instance) {
            errors.append(loader.errorString());
            continue;
        }

        if (auto *backend = qobject_cast<WallpaperThumbnailBackend *>(instance))
            return backend;

        errors.append(QStringLiteral("%1 does not implement the wallpaper thumbnail interface").arg(path));
    }

    qWarning() << "Failed to load wallpaper thumbnail backend:" << errors;
    return nullptr;
}

}

namespace DccWallpaperThumbnail {

QString cachePath(const QString &videoPath)
{
    const QString cacheLocation = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (cacheLocation.isEmpty()) {
        return {};
    }

    const QString fileName = QString::fromLatin1(QCryptographicHash::hash(videoPath.toUtf8(), QCryptographicHash::Md5).toHex()) + ".png";
    return QDir(cacheLocation + "/live-wallpaper-thumbnails").filePath(fileName);
}

QString generate(const QString &videoPath)
{
    const QString outputPath = cachePath(videoPath);
    if (outputPath.isEmpty()) {
        return {};
    }

    if (QFileInfo(outputPath).size() > 0) {
        return outputPath;
    }

    if (!QDir().mkpath(QFileInfo(outputPath).absolutePath())) {
        qWarning() << "Failed to create video thumbnail cache directory:" << outputPath;
        return {};
    }

    DCC_BENCHMARK(QStringLiteral("wallpaper-thumbnail"), "generating-video-thumbnail");
    const QString temporaryPath = outputPath + "." + QUuid::createUuid().toString(QUuid::Id128) + ".tmp";
    static auto *backend = loadThumbnailBackend();
    if (!backend || !backend->generate(videoPath, temporaryPath)) {
        QFile::remove(temporaryPath);
        return {};
    }

    if (QFileInfo(temporaryPath).size() <= 0) {
        QFile::remove(temporaryPath);
        return {};
    }

    if (QFileInfo(outputPath).size() > 0) {
        QFile::remove(temporaryPath);
        return outputPath;
    }

    if (QFile::rename(temporaryPath, outputPath)) {
        return outputPath;
    }

    QFile::remove(temporaryPath);
    return QFileInfo(outputPath).size() > 0 ? outputPath : QString();
}

}
