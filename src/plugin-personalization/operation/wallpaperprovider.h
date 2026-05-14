// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later


#ifndef WALLPAPERPROVIDER_H
#define WALLPAPERPROVIDER_H

#include <QObject>
#include <QPixmap>
#include <QHash>
#include <optional>

#include "operation/personalizationdbusproxy.h"
#include "personalizationmodel.h"
#include "model/wallpapermodel.h"
#include "lastoremanagerjobdbusproxy.h"

using JobInter = LastoreManagerJobDBusProxy;

class InterfaceWorker : public QObject
{
    Q_OBJECT
public:
    explicit InterfaceWorker(PersonalizationDBusProxy *proxy, QObject *parent = nullptr);
    void terminate();

    void getSysBackground();
    void getCustomBackground();
    void getSolodBackground();
    void getLiveBackground();
    QStringList fetchWallpaper(const QString &dir);
    static QString thumbnailCacheDir();

signals:
    void pushBackground(const QList<WallpaperItemPtr> &items, WallpaperType type = WallpaperType::Wallpaper_Sys);
    void pushOneBackground(const WallpaperItemPtr items, WallpaperType type = WallpaperType::Wallpaper_Sys);
    void listFinished();
    void videoThumbnailReady(const QString &id, const QString &thumbnailPath);
public slots:
    void startListBackground(WallpaperType type = WallpaperType::Wallpaper_all);
    void startListOne(const QString &path, WallpaperType type = WallpaperType::Wallpaper_all);
private:
    WallpaperItemPtr createItem(const QString &path, bool del, WallpaperType type);
    void generateVideoThumbnail(const QString &videoPath, const QString &cachePath, const QString &id);
private:
    PersonalizationDBusProxy *m_proxy = nullptr;
    std::atomic_bool m_running = false;
};

class WallpaperProvider : public QObject
{
    Q_OBJECT
public:
    explicit WallpaperProvider(PersonalizationDBusProxy *personalizationProxy, PersonalizationModel *model, QObject *parent = nullptr);
    ~WallpaperProvider();
    void fetchData(WallpaperType type = WallpaperType::Wallpaper_all);
    static bool isColor(const QString &path);
    WallpaperType getWallpaperType(const QString &path);
    void removeWallpaper(const QString &url);
    std::optional<WallpaperItemPtr> findWallpaperItem(const QString &url, WallpaperType type);
    void addWallpaper(const QString &url);
    void installWallpaper(const QString &itemId, WallpaperType type);
    inline WallpaperItemPtr wantToSetWallpaper() const { return m_wantToSetWallpaper; };
    void setWantToSetWallpaper(WallpaperItemPtr wallpaper);

signals:
    void fetchFinish();
    void wantToSetWallpaperStatusChanged(WallpaperInstallStatus status);
    void wantToSetWallpaperProgressChanged(double progress);
    void wantToSetWallpaperChanged(WallpaperItemPtr wallpaper);

private slots:
    void setWallpaper(const QList<WallpaperItemPtr> &items, WallpaperType type = WallpaperType::Wallpaper_Sys);
    void pushWallpaper(WallpaperItemPtr item, WallpaperType type = WallpaperType::Wallpaper_Sys);
    void onWallpaperChangedFromDaemon(const QString &user, uint mode, const QStringList &paths);
    void onVideoThumbnailReady(const QString &id, const QString &thumbnailPath);
    void onJobListChanged(const QList<QDBusObjectPath> &value);

private:
    void createInstallJob(const QString &jobPath, WallpaperItemPtr wallpaperItem);
    WallpaperModel *getModel(WallpaperType type) const;

private:
    QThread *m_workThread = nullptr;
    InterfaceWorker *m_worker = nullptr;
    PersonalizationModel *m_model = nullptr;
    PersonalizationDBusProxy *m_personalizationProxy = nullptr;

    QHash<WallpaperType, QList<WallpaperItemPtr>> m_wallpaperList;
    QHash<QString, JobInter *> m_jobInterMap;
    WallpaperItemPtr m_wantToSetWallpaper;
};

#endif // WALLPAPERPROVIDER_H
