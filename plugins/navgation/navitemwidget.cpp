#include "navitemwidget.h"

#include <QPainter>
#include <QApplication>
#include <QIcon>
#include <QDebug>

NavItemWidget::NavItemWidget(const QString &id, QWidget *parent)
    : QWidget(parent),

      m_id(id),
      m_hover(false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void NavItemWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    QPainter painter(this);

    painter.fillRect(rect(), QColor(255, 255, 255, 255 * (m_hover ? 0.1 : 0.03)));

    const QString &file = QString(":/icons/nav_%1.png").arg(m_id);

    qreal ratio = 1.0;

    const qreal devicePixelRatio = qApp->devicePixelRatio();

    QRect pixRect;
    QPixmap pixmap;

    if (devicePixelRatio > ratio) {
        pixmap.load(qt_findAtNxFile(file, devicePixelRatio, &ratio));

        pixmap = pixmap.scaled(devicePixelRatio / ratio * pixmap.width(),
                               devicePixelRatio / ratio * pixmap.height(),
                               Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        pixmap.setDevicePixelRatio(devicePixelRatio);
        pixRect = QRect(0, 0, pixmap.width() / devicePixelRatio, pixmap.height() / devicePixelRatio);
    } else {
        pixmap.load(file);
        pixRect = pixmap.rect();
    }

    pixRect.moveCenter(rect().center());

    painter.drawPixmap(pixRect, pixmap);
}

void NavItemWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);

    emit itemClicked(m_id);
}

void NavItemWidget::enterEvent(QEvent *e)
{
    m_hover = true;

    QWidget::enterEvent(e);

    emit itemEntered(m_id);

    update();
}

void NavItemWidget::leaveEvent(QEvent *e)
{
    m_hover = false;

    QWidget::leaveEvent(e);

    update();
}
