// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccimageprovider.h"

#include <QImageReader>
#include <QMetaObject>
#include <QThread>
#include <QThreadPool>
#include <QUrl>

namespace dccV25 {
// 4x of 84x54 => 336x216, used as a high-res base thumbnail size
const QSize THUMBNAIL_ICON_SIZE(336, 216);
// Separator for cache key to include size information
const QString CACHE_KEY_SIZE_SEPARATOR = "::SIZE::";

static QSize resolveSize(const QSize &size)
{
    return (size.isValid() && size.width() > 0 && size.height() > 0)
               ? size
               : THUMBNAIL_ICON_SIZE;
}

// ---------------------------------------------------------------------------
// CacheImageResponse – pure QML response container, no loading logic.
// ---------------------------------------------------------------------------
class CacheImageResponse : public QQuickImageResponse
{
    Q_OBJECT
public:
    CacheImageResponse() = default;

    void setImage(const QImage &image) { m_image = image; }
    void emitFinished() { Q_EMIT finished(); }

    QQuickTextureFactory *textureFactory() const override
    {
        return QQuickTextureFactory::textureFactoryForImage(m_image);
    }

private:
    QImage m_image;
};

// ---------------------------------------------------------------------------
// ImageTask – async image loader, runs on thread pool.
// ---------------------------------------------------------------------------
class ImageTask : public QObject, public QRunnable
{
    Q_OBJECT

Q_SIGNALS:
    void imageLoaded(const QString &cacheKey, const QImage &image);

public:
    ImageTask(const QString &id, const QSize &requestedSize, const QString &cacheKey)
        : m_id(id)
        , m_requestedSize(requestedSize)
        , m_cacheKey(cacheKey)
    {
        setAutoDelete(false);
    }

    void run() override
    {
        QUrl url(m_id);
        const QString scheme = url.scheme().toLower();
        QString path;
        if (scheme == "qrc")
            path = ":" + url.path();
        else if (scheme == "file" || url.isLocalFile())
            path = url.toLocalFile();
        else
            path = url.toString();

        QImageReader reader(path);
        if (!reader.canRead()) {
            Q_EMIT imageLoaded(m_cacheKey, QImage());
            return;
        }

        const QSize nativeSize = reader.size();
        if (nativeSize.isValid() && m_requestedSize.isValid()) {
            QSize scaledSize = nativeSize.scaled(m_requestedSize, Qt::KeepAspectRatioByExpanding);
            scaledSize = scaledSize.boundedTo(nativeSize);
            reader.setScaledSize(scaledSize);
        }

        QImage img;
        if (!reader.read(&img)) {
            Q_EMIT imageLoaded(m_cacheKey, QImage());
            return;
        }

        if (img.width() > m_requestedSize.width() || img.height() > m_requestedSize.height()) {
            const QRect dest(QPoint(0, 0), m_requestedSize);
            const QRect src(img.rect().center() - dest.center(), m_requestedSize);
            img = img.copy(src);
        }

        Q_EMIT imageLoaded(m_cacheKey, img);
    }

private:
    QString m_id;
    QSize m_requestedSize;
    QString m_cacheKey;
};

// ---------------------------------------------------------------------------
// DccImageProvider
// ---------------------------------------------------------------------------

DccImageProvider::DccImageProvider()
    : QQuickAsyncImageProvider()
    , m_cache(80)
{
}

DccImageProvider::~DccImageProvider()
{
}

// static
QString DccImageProvider::makeCacheKey(const QString &id, const QSize &size)
{
    const QSize s = resolveSize(size);
    return QString("%1%2%3x%4").arg(id, CACHE_KEY_SIZE_SEPARATOR).arg(s.width()).arg(s.height());
}

void DccImageProvider::submitTask(const QString &id, const QSize &resolvedSize,
                                  const QString &cacheKey, CacheImageResponse *response)
{
    Q_ASSERT(thread() == QThread::currentThread());

    if (m_pendingResponses.contains(cacheKey)) {
        // Task already in flight – just register this response for notification
        if (response)
            m_pendingResponses.insert(cacheKey, response);
        return;
    }

    // Insert the response pointer, or an empty QPointer as an in-flight marker.
    m_pendingResponses.insert(cacheKey, QPointer<CacheImageResponse>(response));

    auto task = new ImageTask(id, resolvedSize, cacheKey);
    connect(task, &ImageTask::imageLoaded, this, &DccImageProvider::onImageLoaded, Qt::QueuedConnection);
    connect(task, &ImageTask::imageLoaded, task, &QObject::deleteLater, Qt::QueuedConnection);
    QThreadPool::globalInstance()->start(task);
}

void DccImageProvider::cacheImage(const QString &id, const QSize &thumbnailSize)
{
    const QString cacheKey = makeCacheKey(id, thumbnailSize);
    const QSize resolved = resolveSize(thumbnailSize);

    const auto work = [this, id, resolved, cacheKey]() {
        if (m_cache.object(cacheKey))
            return;
        submitTask(id, resolved, cacheKey, nullptr);
    };

    QMetaObject::invokeMethod(this, work);
}

QQuickImageResponse *DccImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    const QString cacheKey = makeCacheKey(id, requestedSize);
    const QSize resolved = resolveSize(requestedSize);
    auto response = new CacheImageResponse();
    response->moveToThread(thread());
    QPointer<CacheImageResponse> guardedResponse(response);

    const auto work = [this, guardedResponse, id, resolved, cacheKey]() {
        if (!guardedResponse)
            return;

        if (const QImage *cached = m_cache.object(cacheKey)) {
            // Cache hit: set image and emit finished asynchronously on the provider thread.
            guardedResponse->setImage(*cached);
            guardedResponse->emitFinished();
        } else {
            // Cache miss: submit a loading task. The response will be notified when loading is done.
            submitTask(id, resolved, cacheKey, guardedResponse);
        }
    };

    QMetaObject::invokeMethod(this, work);

    return response;
}

void DccImageProvider::onImageLoaded(const QString &cacheKey, const QImage &image)
{
    Q_ASSERT(thread() == QThread::currentThread());

    if (!image.isNull())
        m_cache.insert(cacheKey, new QImage(image));

    if (!m_pendingResponses.contains(cacheKey))
        return;

    const auto responses = m_pendingResponses.values(cacheKey);
    m_pendingResponses.remove(cacheKey);

    for (const auto &resp : responses) {
        if (resp) {
            if (!image.isNull())
                resp->setImage(image);
            resp->emitFinished();
        }
    }
}

} // namespace dccV25

#include "dccimageprovider.moc"
