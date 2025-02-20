// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later


#ifndef WALLPAPERPROVIDER_H
#define WALLPAPERPROVIDER_H

#include <QObject>
#include <QPixmap>
#include <QMutex>
#include <QMap>

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
    volatile bool m_running = false;
};

class WallpaperWorker : public QObject
{
    Q_OBJECT
public:
    explicit WallpaperWorker(PersonalizationDBusProxy *personalizationProxy, PersonalizationModel *model, QObject *parent = nullptr);
    ~WallpaperWorker();
    void fetchData(WallpaperType type = WallpaperType::Wallpaper_all);
    bool waitWallpaper(int ms = 0) const;
    static bool isColor(const QString &path);
    static WallpaperItemPtr createItem(const QString &path, bool del);
private slots:
    void setWallpaper(const QList<WallpaperItemPtr> &items, WallpaperType type = WallpaperType::Wallpaper_Sys);

private:
    QThread *m_workThread = nullptr;
    InterfaceWorker *m_worker = nullptr;
    PersonalizationModel *m_model = nullptr;
    PersonalizationDBusProxy *m_personalizationProxy = nullptr;
    mutable QMutex m_wallpaperMtx;
    bool fecthing = true;

    QList<WallpaperItemPtr> m_wallpaperList;
    QList<WallpaperItemPtr> m_solidWallpaperList;
};

#endif // WALLPAPERPROVIDER_H
