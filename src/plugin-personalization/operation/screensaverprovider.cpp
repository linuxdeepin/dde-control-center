// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "screensaverprovider.h"
#include "operation/model/wallpapermodel.h"
#include "operation/personalizationdbusproxy.h"
#include "utils.hpp"

#include <QThread>
#include <QSet>
#include <QScreen>
#include <QImageReader>
#include <QPainter>
#include <QMap>

ScreensaverWorker::ScreensaverWorker(PersonalizationDBusProxy *proxy, QObject *parent)
    : QObject(parent)
    , m_proxy(proxy)
{
    
}

void ScreensaverWorker::terminate()
{
    running = false;
}

void ScreensaverWorker::list()
{
    running = true;
    QStringList saverNameList = m_proxy->getAllscreensaver();
    QStringList configurable = m_proxy->ConfigurableItems();

    // Supports parameter setting for multiple screensavers

    int deepin = 0;
    for (const QString &name : saverNameList) {
        // The screensaver with the parameter configuration is placed first
        if (name == DEEPIN_CUSTOM_SCREENSAVER) {
            saverNameList.move(saverNameList.indexOf(name), deepin);
            deepin++;
        }
    }

    if (!running)
        return;

    QList<WallpaperItemPtr> items;
    QMap<QString, QString> imgs;

    for (const QString &name : saverNameList) {
        // remove
        if ("flurry" == name || DEEPIN_CUSTOM_SCREENSAVER == name)
            continue;

        if (!running)
            return;

        auto temp = WallpaperItemPtr(new WallpaperItem);
        items.append(temp);

        QString coverPath = m_proxy->GetScreenSaverCover(name);
        temp->url = name;
        temp->configurable = configurable.contains(name);
        temp->deleteAble = false;
        temp->picPath = coverPath;
        temp->thumbnail = generateThumbnail(coverPath, QSize(THUMBNAIL_ICON_WIDTH, THUMBNAIL_ICON_HEIGHT));

        imgs.insert(name, coverPath);
    }

    emit pushScreensaver(items);
    running = false;
}

void ScreensaverProvider::setScreensaver(const QList<WallpaperItemPtr> &items)
{
    qDebug() << "get screensaver list" << items.size() << "current thread" << QThread::currentThread() << "main:" << qApp->thread();
    m_model->getScreenSaverModel()->resetData(items);}

ScreensaverProvider::ScreensaverProvider(PersonalizationDBusProxy *proxy, PersonalizationModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_proxy(proxy)
{
    workThread = new QThread(this);
    worker = new ScreensaverWorker(proxy);
    worker->moveToThread(workThread);
    workThread->start();

    const static QMap<QString, QMap<QString, QString>> picScreenSaverModesMap {
        {"default", {{"picPath", "qrc:///icons/slideshow_default_preview.webp"}, {"thumbnail", "qrc:///icons/slideshow_default.dci"}}},
    };
    QList<WallpaperItemPtr> items;

    for (auto it = picScreenSaverModesMap.constBegin(); it != picScreenSaverModesMap.constEnd(); ++it) {
        auto temp = WallpaperItemPtr(new WallpaperItem);
        items.append(temp);
        temp->picPath = it.value()["picPath"];
        temp->url = it.key();
        temp->deleteAble = false;
        temp->thumbnail = it.value()["thumbnail"];
    }

    m_model->getPicScreenSaverModel()->resetData(items);

    connect(worker, &ScreensaverWorker::pushScreensaver, this, &ScreensaverProvider::setScreensaver, Qt::QueuedConnection);
}

ScreensaverProvider::~ScreensaverProvider()
{
    worker->terminate();
    workThread->quit();
    workThread->wait(1000);

    if (workThread->isRunning())
        workThread->terminate();

    delete worker;
    worker = nullptr;
}

void ScreensaverProvider::fecthData()
{
    QMetaObject::invokeMethod(worker, "list", Qt::QueuedConnection);
}
