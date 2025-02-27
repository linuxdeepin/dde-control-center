// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later


#ifndef WALLPAPERPROVIDER_H
#define WALLPAPERPROVIDER_H

#include <QObject>
#include <QPixmap>
#include <QMutex>
#include <QMap>
#include <atomic>

#include "operation/personalizationdbusproxy.h"
#include "personalizationmodel.h"
#include "model/wallpapermodel.h"

enum WallpaperType{
    Wallpaper_all = 0,
    Wallpaper_Sys,
    Wallpaper_Custom,
    Wallpaper_Solid,
};

class InterfaceWorker : public QObject
{
    Q_OBJECT
public:
    explicit InterfaceWorker(PersonalizationDBusProxy *proxy, QObject *parent = nullptr);
    void terminate();

    void getSysBackground();
    void getCustomBackground();
    void getSolodBackground();
    QStringList fetchWallpaper(const QString &dir);

signals:
    void pushBackground(const QList<WallpaperItemPtr> &items, WallpaperType type = WallpaperType::Wallpaper_Sys);
public slots:
    void startListBackground(WallpaperType type = WallpaperType::Wallpaper_all);
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
    static WallpaperItemPtr createItem(const QString &path, bool del);
    static WallpaperType getWallpaperType(const QString &path);
private slots:
    void setWallpaper(const QList<WallpaperItemPtr> &items, WallpaperType type = WallpaperType::Wallpaper_Sys);

private:
    QThread *m_workThread = nullptr;
    InterfaceWorker *m_worker = nullptr;
    PersonalizationModel *m_model = nullptr;
    PersonalizationDBusProxy *m_personalizationProxy = nullptr;

    QList<WallpaperItemPtr> m_wallpaperList;
    QList<WallpaperItemPtr> m_solidWallpaperList;
};

#endif // WALLPAPERPROVIDER_H
