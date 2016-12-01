#include "monitorproxywidget.h"
#include "monitor.h"

#include <QPainter>

MonitorProxyWidget::MonitorProxyWidget(Monitor *mon, QWidget *parent)
    : QWidget(parent),

      m_monitor(mon),
      m_movedX(m_monitor->x()),
      m_movedY(m_monitor->y())
{
}

int MonitorProxyWidget::w() const
{
    return m_monitor->w();
}

int MonitorProxyWidget::h() const
{
    return m_monitor->h();
}

void MonitorProxyWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::gray);
    painter.drawRect(rect());

    painter.setPen(Qt::red);
    painter.drawText(10, 15, m_monitor->name());
}
