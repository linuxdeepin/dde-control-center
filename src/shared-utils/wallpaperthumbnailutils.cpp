// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "wallpaperthumbnailutils.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QUuid>

#include <libffmpegthumbnailer/videothumbnailer.h>

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

    const QString temporaryPath = outputPath + "." + QUuid::createUuid().toString(QUuid::Id128) + ".tmp";
    try {
        ffmpegthumbnailer::VideoThumbnailer thumbnailer(480, false, true, 8, false);
        thumbnailer.setThumbnailSize(480, -1);
        thumbnailer.setSeekTime("00:00:01");
        thumbnailer.generateThumbnail(videoPath.toStdString(), Png, temporaryPath.toStdString());
    } catch (const std::exception &error) {
        QFile::remove(temporaryPath);
        qWarning() << "Failed to generate video thumbnail:" << videoPath << error.what();
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
