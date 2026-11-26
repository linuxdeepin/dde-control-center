// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "wallpaperprovider.h"
#include "wallpaperthumbnailutils.h"

#include <gtest/gtest.h>

#include <QDir>
#include <QFile>
#include <QImage>
#include <QScopeGuard>
#include <QTemporaryDir>
#include <QUrl>

TEST(WallpaperProvider, MergesLocalVideosWithoutLosingCatalogDownloads)
{
    QTemporaryDir directory;
    QTemporaryDir cache;
    ASSERT_TRUE(directory.isValid());
    ASSERT_TRUE(cache.isValid());
    const QByteArray previousCache = qgetenv("XDG_CACHE_HOME");
    const bool hadCache = qEnvironmentVariableIsSet("XDG_CACHE_HOME");
    const auto restoreCache = qScopeGuard([&] {
        if (hadCache)
            qputenv("XDG_CACHE_HOME", previousCache);
        else
            qunsetenv("XDG_CACHE_HOME");
    });
    qputenv("XDG_CACHE_HOME", cache.path().toUtf8());

    auto writeFile = [](const QString &path, const QByteArray &contents) {
        QFile file(path);
        return file.open(QIODevice::WriteOnly) && file.write(contents) == contents.size();
    };
    QImage thumbnail(8, 8, QImage::Format_RGB32);
    thumbnail.fill(Qt::blue);
    const QString preview = directory.filePath("preview.png");
    ASSERT_TRUE(thumbnail.save(preview));

    const QStringList videoNames = {
        "official.mp4", "local.Mp4", "local.mov", "local.avi", "local.webm", "local.mkv"
    };
    QStringList localUrls;
    for (const auto &name : videoNames) {
        const QString path = directory.filePath(name);
        ASSERT_TRUE(writeFile(path, {}));
        const QString cachedThumbnail = DccWallpaperThumbnail::cachePath(path);
        ASSERT_TRUE(QDir().mkpath(QFileInfo(cachedThumbnail).absolutePath()));
        ASSERT_TRUE(thumbnail.save(cachedThumbnail));
        localUrls.append(QUrl::fromLocalFile(path).toString());
    }
    ASSERT_TRUE(writeFile(directory.filePath("notes.txt"), "not a wallpaper"));
    ASSERT_TRUE(QDir().mkpath(directory.filePath("directory.mp4")));

    const QString metadata = directory.filePath("metadata.json");
    ASSERT_TRUE(writeFile(metadata, R"([
        {"id":"official","path":"./official.mp4","thumbnail":"preview.png","packageName":"official-package"},
        {"id":"duplicate","path":"official.mp4","packageName":"duplicate-package"},
        {"id":"download","path":"download.mp4","thumbnail":"preview.png","packageName":"download-package"}
    ])"));

    InterfaceWorker worker(nullptr);
    QList<WallpaperItemPtr> items;
    QObject::connect(&worker, &InterfaceWorker::pushBackground, &worker,
                     [&](const QList<WallpaperItemPtr> &result, WallpaperType type) {
                         EXPECT_EQ(type, Wallpaper_Live);
                         items = result;
                     });
    worker.getLiveBackground(directory.path());

    QStringList urls;
    WallpaperItemPtr official;
    WallpaperItemPtr download;
    for (const auto &item : items) {
        urls.append(item->url);
        EXPECT_EQ(item->type, Wallpaper_Live);
        if (item->packageName == "official-package")
            official = item;
        if (item->packageName == "download-package")
            download = item;
    }
    QStringList expectedUrls = localUrls;
    expectedUrls.append(QUrl::fromLocalFile(directory.filePath("download.mp4")).toString());
    urls.sort();
    expectedUrls.sort();
    EXPECT_EQ(urls, expectedUrls);
    ASSERT_TRUE(official);
    EXPECT_EQ(official->installStatus, Download_Installed);
    EXPECT_EQ(official->thumbnail, QUrl::fromLocalFile(preview).toString());
    ASSERT_TRUE(download);
    EXPECT_EQ(download->installStatus, Download_NotInstalled);
    EXPECT_EQ(download->installProgress, 0);
    EXPECT_EQ(download->thumbnail, QUrl::fromLocalFile(preview).toString());
    for (const auto &item : items) {
        if (item != official && item != download) {
            EXPECT_TRUE(item->packageName.isEmpty());
            EXPECT_EQ(item->installStatus, Download_Installed);
            EXPECT_EQ(item->thumbnail, QUrl::fromLocalFile(
                DccWallpaperThumbnail::cachePath(QUrl(item->url).toLocalFile())).toString());
        }
    }

    auto expectLocalVideos = [&] {
        items.clear();
        worker.getLiveBackground(directory.path());
        QStringList actualUrls;
        for (const auto &item : items) {
            actualUrls.append(item->url);
            EXPECT_EQ(item->installStatus, Download_Installed);
            EXPECT_TRUE(item->packageName.isEmpty());
        }
        actualUrls.sort();
        QStringList expected = localUrls;
        expected.sort();
        EXPECT_EQ(actualUrls, expected);
    };
    ASSERT_TRUE(QFile::remove(metadata));
    expectLocalVideos();
    ASSERT_TRUE(writeFile(metadata, "invalid json"));
    expectLocalVideos();
    ASSERT_TRUE(writeFile(metadata, "{}"));
    expectLocalVideos();
    ASSERT_TRUE(QFile::remove(metadata));
    ASSERT_TRUE(QDir().mkpath(metadata));
    expectLocalVideos();
}
