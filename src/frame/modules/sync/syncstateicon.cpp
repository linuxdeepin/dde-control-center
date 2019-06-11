#include "syncstateicon.h"

#include <QMatrix>
#include <QVBoxLayout>
#include <QPainter>

using namespace dcc;
using namespace dcc::cloudsync;

SyncStateIcon::SyncStateIcon(QWidget* parent) : QLabel(parent)
{
    m_rotateAni.setStartValue(0);
    m_rotateAni.setEndValue(360);
    m_rotateAni.setDuration(5000);
    m_rotateAni.setLoopCount(-1);

    connect(&m_rotateAni, &QVariantAnimation::valueChanged, this,
            [=](const QVariant& value) -> void {
                m_rotateRatio = value.toDouble();
                update();
            });
}

SyncStateIcon::~SyncStateIcon() {}

void SyncStateIcon::setRotatePixmap(const QPixmap& pixmap)
{
    m_rotatePixmap = pixmap;
    update();
}

void SyncStateIcon::play()
{
    m_rotateAni.start();
}

void SyncStateIcon::stop()
{
    m_rotateAni.stop();

    m_rotateRatio = 0;
}

void SyncStateIcon::paintEvent(QPaintEvent* event)
{
    QLabel::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);

    const QSize pixmapSize { m_rotatePixmap.size() / devicePixelRatioF() };
    const QPointF pixmapCenter { static_cast<qreal>(pixmapSize.width() / 2), static_cast<qreal>(pixmapSize.height() / 2) };
    const QPoint offset { rect().center() - pixmapCenter.toPoint() };
    const QRect r(offset, pixmapSize);
    painter.translate(pixmapCenter + offset);
    painter.rotate(m_rotateRatio);
    painter.translate(-pixmapCenter - offset);
    painter.drawPixmap(r, m_rotatePixmap);
}
