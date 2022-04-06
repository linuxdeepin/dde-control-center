/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "monitorproxywidget.h"
#include "monitor.h"
#include "monitorindicator.h"
#include "displaymodel.h"

#include <QPainter>
#include <QMouseEvent>

using namespace dcc::display;

MonitorProxyWidget::MonitorProxyWidget(Monitor *mon, DisplayModel *model, QWidget *parent)
    : QWidget(parent)
    , m_monitor(mon)
    , m_model(model)
    , m_movedX(m_monitor->x())
    , m_movedY(m_monitor->y())
    , m_underMouseMove(false)
{
    connect(m_monitor, &Monitor::xChanged, this, &MonitorProxyWidget::setMovedX);
    connect(m_monitor, &Monitor::yChanged, this, &MonitorProxyWidget::setMovedY);
    this->setVisible(mon->enable());
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
    painter.setPen(Qt::darkGray);
    painter.drawRect(r);

    const QFontMetrics fm(painter.font());
    const int width = fm.boundingRect(m_monitor->name()).width();
    painter.setPen(Qt::white);
    if (!m_model->isMerge()) {
        int xstart = r.width() - width - 20;
        if (xstart > 0) {
            painter.drawText(r.width() - width - 20, 30, m_monitor->name());
        } else {
            painter.drawText(10, 30, m_monitor->name());
        }
    }

    // draw dock pattern if it's primary screen
    if (m_monitor->isPrimary())
    {
        const int radius = 5;
        QRectF dockRect = r;
        dockRect.setTop(r.bottom() - 15);
        dockRect.setRight(r.width() - r.width() / 4);
        dockRect.setLeft(r.width() / 4);
        dockRect.moveTop(dockRect.top() + radius);

        painter.setPen(Qt::transparent);
        painter.setBrush(Qt::white);
        painter.drawRoundedRect(dockRect, radius, radius);
    }

    // draw blue border if under mouse control
    if (m_underMouseMove)
    {
        QPen pen(QColor("#2ca7f8"));
        pen.setWidth(4);
        painter.setPen(pen);
        painter.setBrush(Qt::transparent);
        painter.drawRect(rect());
    }
}

void MonitorProxyWidget::mousePressEvent(QMouseEvent *e)
{
    m_lastPos = e->globalPos();

    m_underMouseMove = true;

    Q_EMIT requestMonitorPress(m_monitor);

    update();
}

void MonitorProxyWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (m_model->isMerge())
        return;

    if (!(e->buttons() & Qt::LeftButton))
        return;

    this->parentWidget()->update();
    move(pos() + e->globalPos() - m_lastPos);
    m_lastPos = e->globalPos();

    //解决设置1.25缩放，自定义拖动dp和edp缩略图有残影问题
    Q_EMIT requestUpdateWidget();
}

void MonitorProxyWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (!m_model->isMerge())
        Q_EMIT requestApplyMove(this);

    m_underMouseMove = false;

    Q_EMIT requestMonitorRelease(m_monitor);

    update();

    QWidget::mouseReleaseEvent(e);
}
