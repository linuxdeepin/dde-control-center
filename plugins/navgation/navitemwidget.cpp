#include "navitemwidget.h"

#include <QPainter>

NavItemWidget::NavItemWidget(const QString &id, QWidget *parent)
    : QWidget(parent),

      m_id(id)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(80);
}

void NavItemWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    QPainter painter(this);
    painter.fillRect(rect(), QColor(255, 255, 255, 255 * .2));
    painter.drawText(rect(), Qt::AlignCenter, m_id);
}

void NavItemWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);

    emit itemClicked(m_id);
}
