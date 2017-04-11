#include "navitemwidget.h"

#include <QPainter>

NavItemWidget::NavItemWidget(const QString &id, QWidget *parent)
    : QWidget(parent),

      m_id(id)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void NavItemWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    QPainter painter(this);

    const QPixmap pixmap = QPixmap(QString(":/icons/nav_%1.png").arg(m_id));
    painter.fillRect(rect(), QColor(255, 255, 255, 255 * .2));
    painter.drawPixmap(rect().center() - pixmap.rect().center(), pixmap);
}

void NavItemWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);

    emit itemClicked(m_id);
}

void NavItemWidget::enterEvent(QEvent *e)
{
    QWidget::enterEvent(e);

    emit itemEntered(m_id);
}
