// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCIMAGEPROVIDER_H
#define DCCIMAGEPROVIDER_H
#include <QCache>
#include <QMutex>
#include <QObject>
#include <QQuickAsyncImageProvider>
#include <QThreadPool>

namespace dccV25 {
class CacheImageResponse;

class DccImageProvider : public QQuickAsyncImageProvider
{
    Q_OBJECT
public:
    explicit DccImageProvider();

    QImage *cacheImage(const QString &id, const QSize &thumbnailSize);
    QImage *cacheImage(const QString &id, const QSize &thumbnailSize, CacheImageResponse *response, const QSize &requestedSize);
    bool insert(const QString &id, QImage *img);

    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;

private:
    QCache<QString, QImage> m_cache;
    QThreadPool *m_threadPool;
    QMutex m_mutex;
};
} // namespace dccV25
#endif // DCCIMAGEPROVIDER_H
