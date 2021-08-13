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
#include "displaymodel.h"

#include <QPainter>
#include <QMouseEvent>
#include <QScroller>
#include <QScrollArea>

using namespace dcc::display;

MonitorProxyWidget::MonitorProxyWidget(Monitor *mon, DisplayModel *model)
    : m_monitor(mon)
    , m_model(model)
    , m_movedX(m_monitor->x())
    , m_movedY(m_monitor->y())
    , m_center(QPointF(0,0))
    , m_edge(QPointF(0,0))
{

    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);

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


void MonitorProxyWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();
    QRectF r(0, 0, abs(m_edge.x()), abs(m_edge.y()));
    const int arc = 8;
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(QColor("#5f5f5f"));
    painter->setPen(QColor("#2e2e2e"));
    painter->drawRoundedRect(r, arc, arc);
    painter->setClipRect(this->boundingRect());

    const QFontMetrics fm(painter->font());
    const int width = fm.boundingRect(m_monitor->name()).width();
    painter->setPen(Qt::white);
    if (m_model->displayMode() != MERGE_MODE) {
        qreal xstart = r.width() - width - 20;
        if (xstart > 0) {
            painter->drawText(r.width() - width - 20, 30, m_monitor->name());
        } else {
            painter->drawText(10, 30, m_monitor->name());
        }
    }

    // draw dock pattern if it's primary screen
    if (m_model->displayMode() != MERGE_MODE && m_monitor->isPrimary()) {
        const int radius = 5;
        QRectF dockRect = r;
        dockRect.setTop(r.bottom() - 15);
        dockRect.setRight(r.width() - r.width() / 4);
        dockRect.setLeft(r.width() / 4);
        dockRect.moveTop(dockRect.top() + radius);
        painter->setBrush(Qt::white);
        painter->drawRoundedRect(dockRect, radius, radius);


        // draw blue border if the mode is EXTEND_MODE
        QPen penWhite(Qt::white);
        penWhite.setWidth(3);
        painter->setPen(penWhite);
        painter->setBrush(Qt::transparent);
        painter->drawRoundedRect(r, arc, arc);

        QPen pen(QColor("#2ca7f8"));
        pen.setWidth(2);
        painter->setPen(pen);
        painter->setBrush(Qt::transparent);
        painter->drawRoundedRect(r, arc, arc);
    }
    painter->restore();
}

void MonitorProxyWidget::mousePressEvent(QGraphicsSceneMouseEvent *e)
{

    if (m_model->displayMode() == EXTEND_MODE) {
        Q_EMIT requestMonitorPress(m_monitor);
    }
    QGraphicsItem::mousePressEvent(e);
}

void MonitorProxyWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    if (m_model->displayMode() == MERGE_MODE)
        return;

    if (!(e->buttons() & Qt::LeftButton))
        return;

    QGraphicsItem::mouseMoveEvent(e);
}

void MonitorProxyWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    if (m_model->displayMode() == EXTEND_MODE) {
        Q_EMIT requestApplyMove(this);
        Q_EMIT requestMonitorRelease(m_monitor);
    }

    QGraphicsItem::mouseReleaseEvent(e);
}

void MonitorProxyWidget::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    //增加选中的效果
    QGraphicsItem::focusInEvent(event);
}
void MonitorProxyWidget::focusOutEvent(QFocusEvent *event) 
{
    Q_UNUSED(event);
    //增加选中的效果
    QGraphicsItem::focusOutEvent(event);
}

QRectF MonitorProxyWidget::boundingRect() const
{
    return QRectF(0 - abs(m_edge.x()), 0 - abs(m_edge.y()), abs(m_edge.x()) * 2, abs(m_edge.y()) * 2);
}

QRectF MonitorProxyWidget::bufferboundingRect() const
{
    return QRectF(m_center.x() - abs(m_edge.x()), m_center.y() - abs(m_edge.y()), abs(m_edge.x()) * 2, abs(m_edge.y()) * 2).adjusted(-10,-10,10,10);
}

QRectF MonitorProxyWidget::adsorptionbufferboundingRect() const
{
    return QRectF(m_center.x() - abs(m_edge.x()), m_center.y() - abs(m_edge.y()), abs(m_edge.x()) * 2, abs(m_edge.y()) * 2).adjusted(-10,-10,10,10);/*.adjusted(-m_dx,-m_dy,m_dx,m_dy)*/
}
