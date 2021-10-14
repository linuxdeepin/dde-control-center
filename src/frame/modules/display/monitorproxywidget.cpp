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
    , m_movingX(0.0)
    , m_movingY(0.0)
    , m_edge(QPointF(0, 0))
    , m_startPos(QPointF(0, 0))
    , m_endPos(QPointF(0, 0))
    , m_preCenter(QPointF(0, 0))
    , m_preEdge(QPointF(0, 0))
    , m_selected(false)
    , m_isMoved(false) {
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
}

void MonitorProxyWidget::setMovedX(const int x) {
    m_movedX = x;
}
void MonitorProxyWidget::setMovedY(const int y) {
    m_movedY = y;
}

int MonitorProxyWidget::w() const {
    return m_monitor->w();
}

int MonitorProxyWidget::h() const {
    return m_monitor->h();
}

const QString MonitorProxyWidget::name() const {
    return m_monitor->name();
}

//先对与原始坐标的移动之后的位置信息
void MonitorProxyWidget::executeMoveBy(qreal dx, qreal dy) {
    this->moveBy(dx, dy);
    m_movingX = m_movingX + dx;
    m_movingY = m_movingY + dy;
}

void MonitorProxyWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();
    QRectF r = this->boundingRect();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(QColor("#5f5f5f"));
    painter->setPen(QColor("#2e2e2e"));

    painter->drawRect(r);
    painter->setClipRect(r);

    QFont font = painter->font() ;
    font.setPointSize(108);
    painter->setFont(font);

    const QFontMetrics fm(font);
    const int width = fm.boundingRect(m_monitor->name()).width();
    const int height = fm.boundingRect(m_monitor->name()).height();

    painter->setPen(Qt::white);
    if (!m_model->isMerge()) {
        int xstart = r.width() - width - 100;
        if (xstart <= 0) {
            xstart = 10;
        }
        painter->drawText(xstart, height, m_monitor->name());

        // draw dock pattern if it's primary screen
        if (m_monitor->isPrimary()) {
            const int radius = 50;
            QRectF dockRect = r;
            dockRect.setTop(r.bottom() - 150);
            dockRect.setRight(r.width() - r.width() / 4);
            dockRect.setLeft(r.width() / 4);
            dockRect.moveTop(dockRect.top() + radius);
            painter->setBrush(Qt::white);
            painter->drawRoundedRect(dockRect, radius, radius);
        }

        //根据是否焦点绘制选中状态
        if (m_selected) {
            // draw blue border if the mode is EXTEND_MODE
            QPen penWhite(Qt::white);
            penWhite.setWidth(3);
            painter->setPen(penWhite);
            painter->setBrush(Qt::transparent);
            painter->drawRect(r);

            QPen pen(QColor("#2ca7f8"));
            pen.setWidth(2);
            painter->setPen(pen);
            painter->setBrush(Qt::transparent);
            painter->drawRect(r);
        }
    }

    painter->restore();
}

void MonitorProxyWidget::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (!m_model->isMerge()) {
        m_startPos = this->pos();
        Q_EMIT requestMonitorPress(m_monitor);
    }
    QGraphicsItem::mousePressEvent(event);
}

void MonitorProxyWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent(event);

    if (!m_model->isMerge()) {
        m_isMoved = true;
        Q_EMIT requestMouseMove(this);
    }
}

void MonitorProxyWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (!m_model->isMerge()) {
        if (m_isMoved) {
            m_endPos = this->pos();
            QPointF pos = m_endPos - m_startPos;
            m_movingX = pos.x() + m_movingX;
            m_movingY = pos.y() + m_movingY;

            Q_EMIT requestMonitorRelease(m_monitor);

            m_isMoved = false;
        }
    }

    QGraphicsItem::mouseReleaseEvent(event);
}

void MonitorProxyWidget::focusInEvent(QFocusEvent *event) {
    //增加选中的效果
    m_selected = true;

    QGraphicsItem::focusInEvent(event);
}

void MonitorProxyWidget::focusOutEvent(QFocusEvent *event) {
    //增加选中的效果
    m_selected = false;

    QGraphicsItem::focusOutEvent(event);
}

void MonitorProxyWidget::keyPressEvent(QKeyEvent *event) {
    QGraphicsItem::keyPressEvent(event);

    if (!m_model->isMerge()) {
        Q_EMIT requestKeyPress(this, event->key());
    }
}

QRectF MonitorProxyWidget::boundingRect() const {
    return QRectF(0, 0, abs(m_edge.x()), abs(m_edge.y()));
}

QRectF MonitorProxyWidget::bufferboundingRect() const {
    int wbuf = m_edge.x() / 10;
    int hbuf = m_edge.y() / 10;
    return boundingRect().adjusted(-wbuf, -hbuf, wbuf, hbuf);
}

//外扩0.05个像素, 规避由于缩放导致精度丢失或者坐标值完全一致的情况下不能判定为相交的情况
QRectF MonitorProxyWidget::boundingRectEx() const {
    return boundingRect().adjusted(-0.05, -0.05, 0.05, 0.05);
}

QRectF MonitorProxyWidget::justIntersectRect() const {
    return boundingRect().adjusted(1, 1, -1, -1);
}
