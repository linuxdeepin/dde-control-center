// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QThread>
#include <QScreen>
#include <QImageReader>
#include <QPainter>
#include <QJsonDocument>
#include <QJsonArray>
#include <QImage>
#include <QLoggingCategory>
#include <QUrl>

#include "wallpaperworker.h"
#include "operation/personalizationdbusproxy.h"

Q_LOGGING_CATEGORY(DdcPersonalizationWallpaperWorker, "dcc-personalization-wallpaper-worker")

WallpaperWorker::WallpaperWorker(PersonalizationDBusProxy *PersonalizationDBusProxy, WallpaperModel *wallPaperModel, QObject *parent) : QObject(parent)
{
    m_workThread = new QThread(this);

    m_personalizationProxy = PersonalizationDBusProxy;
    m_worker = new InterfaceWorker(PersonalizationDBusProxy);
    m_model = wallPaperModel;
    m_worker->moveToThread(m_workThread);
    m_workThread->start();

    // DirectConnection to set datas
    connect(m_worker, &InterfaceWorker::pushBackground, this, &WallpaperWorker::setWallpaper, Qt::DirectConnection);

    connect(m_worker, &InterfaceWorker::pushBackgroundStat, this, &WallpaperWorker::updateWallpaper, Qt::QueuedConnection);
}

WallpaperWorker::~WallpaperWorker()
{
    m_worker->terminate();
    m_workThread->quit();
    m_workThread->wait(1000);

    if (m_workThread->isRunning()) {
        m_workThread->terminate();
    }

    delete m_worker;
    m_worker = nullptr;
}

void WallpaperWorker::fetchData()
{
    // get picture
    if (m_wallpaperMtx.tryLock(1)) {
        fecthing = true;
        QMetaObject::invokeMethod(m_worker, "onListBackground", Qt::QueuedConnection);
    } else {
        qWarning() << "wallpaper is locked...";
    }
}

bool WallpaperWorker::waitWallpaper(int ms) const
{
    bool ret = m_wallpaperMtx.tryLock(ms);
    if (ret)
        m_wallpaperMtx.unlock();
    return ret;
}

QList<ItemNodePtr> WallpaperWorker::pictures() const
{
    QList<ItemNodePtr> list;
    for (const ItemNodePtr & ptr : m_wallpaper) {
        if (!isColor(ptr->item))
            list.append(ptr);
    }
    return list;
}

QList<ItemNodePtr> WallpaperWorker::colors() const
{
    QList<ItemNodePtr> list;
    for (const ItemNodePtr & ptr : m_wallpaper) {
        if (isColor(ptr->item))
            list.append(ptr);
    }
    return list;
}

bool WallpaperWorker::isColor(const QString &path)
{
    // these dirs save solid color wallpapers.
    return path.startsWith("/usr/share/wallpapers/custom-solidwallpapers")
            || path.startsWith("/usr/share/wallpapers/deepin-solidwallpapers");
}

void WallpaperWorker::setWallpaper(const QList<ItemNodePtr> &items)
{
    qCDebug(DdcPersonalizationWallpaperWorker) << "get wallpaper list" << items.size() << "current thread" << QThread::currentThread() << "main:" << qApp->thread();
    m_wallpaper = items;
    m_wallpaperMap.clear();
    for(auto it : items) {
        m_wallpaperMap.insert(it->item, it);
    }

    m_wallpaperMtx.unlock();
    fecthing = false;
    m_model->resetData(m_wallpaper);
}

void WallpaperWorker::updateWallpaper(const QMap<QString, bool> &stat)
{
    qCDebug(DdcPersonalizationWallpaperWorker) << "update background stat" << stat.size();
    for (auto it = stat.begin(); it != stat.end(); ++it) {
        if (auto ptr = m_wallpaperMap.value(it.key()))
            ptr->deletable = it.value();
    }
}

ItemNodePtr WallpaperWorker::createItem(const QString &id, bool del)
{
    ItemNodePtr ret;
    if (id.isEmpty())
        return ret;

    ret = ItemNodePtr(new ItemNode{id, true, del});
    return ret;
}

QList<ItemNodePtr> InterfaceWorker::processListReply(const QString &reply)
{
    QList<ItemNodePtr> result;
    QJsonDocument doc = QJsonDocument::fromJson(reply.toUtf8());
    if (doc.isArray()) {
        QJsonArray arr = doc.array();
        foreach (QJsonValue val, arr) {

            if (!m_running)
                return result;

            QJsonObject obj = val.toObject();
            QString id = obj["Id"].toString(); //url
            // id = covertUrlToLocalPath(id);
            if (auto ptr = WallpaperWorker::createItem(id, obj["Deletable"].toBool()))
                result.append(ptr);
        }
    }

    return result;
}

InterfaceWorker::InterfaceWorker(PersonalizationDBusProxy *proxy, QObject *parent)
    : QObject(parent)
    , m_proxy(proxy)
{

}

void InterfaceWorker::terminate()
{
    m_running = false;
}

bool InterfaceWorker::generateThumbnail(const QString &path, const QSize &size, bool &pic, QVariant &val)
{
    bool ret = true;

    QImage image(path);
    if (WallpaperWorker::isColor(path)) {
        pic = false;
        if (image.sizeInBytes() > 0)
            val = QVariant::fromValue(image.pixelColor(0, 0));
        else
            ret = false;
    } else {
        QPixmap pix = QPixmap::fromImage(image.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        const QRect r(QPoint(0, 0), size);

        if (pix.width() > size.width() || pix.height() > size.height())
            pix = pix.copy(QRect(pix.rect().center() - r.center(), size));

        pic = true;
        val = QVariant::fromValue(pix);
    }

    return ret;
}

void InterfaceWorker::onListBackground()
{
    m_running = true;

    QList<ItemNodePtr> result = processListReply(m_proxy->List("background"));
    if (m_running) {
        emit pushBackground(result);

        QStringList paths;
        for (const ItemNodePtr &it : result)
            paths.append(it->item);
    }

    m_running = false;
}
