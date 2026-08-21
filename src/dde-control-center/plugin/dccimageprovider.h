// SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCIMAGEPROVIDER_H
#define DCCIMAGEPROVIDER_H
#include <QCache>
#include <QImage>
#include <QMultiMap>
#include <QObject>
#include <QPointer>
#include <QQuickAsyncImageProvider>

namespace dccV25 {
class CacheImageResponse;

class DccImageProvider : public QQuickAsyncImageProvider
{
    Q_OBJECT
public:
    explicit DccImageProvider();
    ~DccImageProvider() override;

    void cacheImage(const QString &id, const QSize &thumbnailSize);
    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;

private Q_SLOTS:
    void onImageLoaded(const QString &cacheKey, const QImage &image);
    
private:
    static QString makeCacheKey(const QString &id, const QSize &size);
    void submitTask(const QString &id, const QSize &resolvedSize, const QString &cacheKey,
                    CacheImageResponse *response);

    QCache<QString, QImage> m_cache;
    QMultiMap<QString, QPointer<CacheImageResponse>> m_pendingResponses;
};
} // namespace dccV25
#endif // DCCIMAGEPROVIDER_H
