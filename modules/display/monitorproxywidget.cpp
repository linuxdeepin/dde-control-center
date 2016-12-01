#include "monitorproxywidget.h"
#include "monitor.h"

#include <QPainter>
#include <QMouseEvent>

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

const QString MonitorProxyWidget::name() const
{
    return m_monitor->name();
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

void MonitorProxyWidget::mousePressEvent(QMouseEvent *e)
{
    m_lastPos = e->globalPos();
}

void MonitorProxyWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (!(e->buttons() & Qt::LeftButton))
        return;

    move(pos() + e->globalPos() - m_lastPos);
    m_lastPos = e->globalPos();
}

void MonitorProxyWidget::mouseReleaseEvent(QMouseEvent *)
{
    emit requestApplyMove(this);
}
