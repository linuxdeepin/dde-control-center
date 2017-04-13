#include "navitemwidget.h"

#include <QPainter>

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
    const QPixmap pixmap = QPixmap(QString(":/icons/nav_%1.png").arg(m_id));
    painter.drawPixmap(rect().center() - pixmap.rect().center(), pixmap);
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
