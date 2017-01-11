#include "monitorproxywidget.h"
#include "monitor.h"

#include <QPainter>
#include <QMouseEvent>

using namespace dcc::display;

MonitorProxyWidget::MonitorProxyWidget(Monitor *mon, QWidget *parent)
    : QWidget(parent),

      m_monitor(mon),
      m_movedX(m_monitor->x()),
      m_movedY(m_monitor->y())
{
    connect(m_monitor, &Monitor::xChanged, this, &MonitorProxyWidget::setMovedX);
    connect(m_monitor, &Monitor::yChanged, this, &MonitorProxyWidget::setMovedY);
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
    const QRect r(rect());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(r, Qt::gray);
//    painter.fillRect(r, m_monitor->isPrimary() ? Qt::blue : Qt::gray);
    painter.setPen(Qt::darkGray);
    painter.drawRect(r);

    const QFontMetrics fm(painter.font());
    const int width = fm.boundingRect(m_monitor->name()).width();
    painter.setPen(Qt::white);
    painter.drawText(r.width() - width - 20, 30, m_monitor->name());

    if (!m_monitor->isPrimary())
        return;

    const int radius = 5;
    QRectF dockRect = r;
    dockRect.setTop(r.bottom() - 15);
    dockRect.setRight(r.width() - r.width() / 4);
    dockRect.setLeft(r.width() / 4);
    dockRect.moveTop(dockRect.top() + radius);

    painter.setPen(Qt::transparent);
    painter.setBrush(Qt::darkGray);
//    painter.fillRect(dockRect, Qt::red);
    painter.drawRoundedRect(dockRect, radius, radius);
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
