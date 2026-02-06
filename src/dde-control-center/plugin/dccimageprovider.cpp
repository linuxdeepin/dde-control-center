// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccimageprovider.h"

#include <QMutexLocker>
#include <QMetaObject>

namespace dccV25 {
// 4x of 84x54 => 336x216, used as a high-res base thumbnail size
const QSize THUMBNAIL_ICON_SIZE(336, 216);

class CacheImageResponse : public QQuickImageResponse
{
public:
    CacheImageResponse(const QString &id, const QSize &requestedSize, DccImageProvider *provider)
        : QQuickImageResponse()
    {
        QImage *img = provider->cacheImage(id, requestedSize, this, requestedSize);
        if (img) {
            // Image is already scaled to the proper size, no need to rescale here.
            setImage(*img, QSize());
        }
    }

    QQuickTextureFactory *textureFactory() const override { return QQuickTextureFactory::textureFactoryForImage(m_image); }

    void setImage(const QImage &image, const QSize &requestedSize)
    {
        QMetaObject::invokeMethod(this, [this, image, requestedSize]() {
            m_image = requestedSize.isValid() ? image.scaled(requestedSize) : image;
            Q_EMIT finished();
        }, Qt::QueuedConnection);
    }

private:
    QImage m_image;
};

class ImageTask : public QRunnable
{
public:
    explicit ImageTask(const QString &id, const QSize &thumbnailSize, DccImageProvider *provider, CacheImageResponse *response, const QSize &requestedSize)
        : QRunnable()
        , m_id(id)
        , m_size(thumbnailSize.isValid() ? thumbnailSize : THUMBNAIL_ICON_SIZE)
        , m_requestedSize(requestedSize)
        , m_provider(provider)
        , m_response(response)
    {
    }

protected:
    void run() override;

protected:
    QString m_id;
    QSize m_size;
    QSize m_requestedSize;
    QPointer<DccImageProvider> m_provider;
    QPointer<CacheImageResponse> m_response;
};

void ImageTask::run()
{
    QImage originalImage;
    QUrl url(m_id);
    QString scheme = url.scheme().toLower();

    QString path;
    if (scheme == "qrc") {
        path = ":" + url.path();
    }
    else if (scheme == "file" || url.isLocalFile()) {
        path = url.toLocalFile();
    }
    else {
        path = url.toString();
    }

    if (originalImage.load(path)) {
        QSize targetSize = m_requestedSize.isValid() ? m_requestedSize : m_size;
        if (!targetSize.isValid()) {
            targetSize = THUMBNAIL_ICON_SIZE;
        }

        QImage img = originalImage.scaled(targetSize,
                                          Qt::KeepAspectRatioByExpanding,
                                          Qt::SmoothTransformation);
        if (img.width() > targetSize.width() || img.height() > targetSize.height()) {
            const QRect r(QPoint(0, 0), targetSize);
            const QRect srcRect(img.rect().center() - r.center(), targetSize);
            img = img.copy(srcRect);
        }

        if (m_provider && m_response) {
            QImage *heapImage = new QImage(std::move(img));
            if (m_provider->insert(m_id, heapImage)) {
                // Image already at target size, no further scaling needed.
                m_response->setImage(*heapImage, QSize());
            } else {
                delete heapImage;
            }
        }
    }
}

DccImageProvider::DccImageProvider()
    : QQuickAsyncImageProvider()
    , m_cache(80)
    , m_threadPool(new QThreadPool(this))
{
}

DccImageProvider::~DccImageProvider()
{
    if (m_threadPool) {
        m_threadPool->waitForDone();
    }
}

QImage *DccImageProvider::cacheImage(const QString &id, const QSize &thumbnailSize)
{
    return cacheImage(id, thumbnailSize, new CacheImageResponse(id, QSize(), this), QSize());
}

QImage *DccImageProvider::cacheImage(const QString &id, const QSize &thumbnailSize, CacheImageResponse *response, const QSize &requestedSize)
{
    QMutexLocker<QMutex> locker(&m_mutex);
    if (m_cache.contains(id)) {
        return m_cache.object(id);
    }
    m_threadPool->start(new ImageTask(id, thumbnailSize, this, response, requestedSize));
    return nullptr;
}

QQuickImageResponse *DccImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    return new CacheImageResponse(id, requestedSize, this);
}

bool DccImageProvider::insert(const QString &id, QImage *img)
{
    QMutexLocker<QMutex> locker(&m_mutex);
    return m_cache.insert(id, img);
}

} // namespace dccV25
