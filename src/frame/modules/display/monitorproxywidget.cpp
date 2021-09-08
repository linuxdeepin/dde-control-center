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
    , m_preCenter(QPointF(0,0))
    , m_selected(false)
    , m_isMoving(false)
{
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
}

void MonitorProxyWidget::setMovedX(const int x)
{
    m_movedX = x;
}
void MonitorProxyWidget::setMovedY(const int y)
{
    m_movedY = y;
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
    QRectF r = this->boundingRect();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(QColor("#5f5f5f"));
    painter->setPen(QColor("#2e2e2e"));
    auto radius = std::max(r.width(), r.height()) / 20;
    painter->drawRoundedRect(r, radius, radius);
    painter->setClipRect(r);

    QFont font ("Microsoft YaHei", 100, 20);
    painter->setFont(font);
    const QFontMetrics fm(painter->font());
    const int width = fm.boundingRect(name()).width();
    const int height = fm.boundingRect(name()).height();
    painter->setPen(Qt::white);
    if (m_model->displayMode() != MERGE_MODE) {
        painter->drawText(QRectF(r.x() + r.width() - width - height, r.y() + height, width, height), Qt::AlignCenter, name());
    }

    // draw dock pattern if it's primary screen
    if (m_model->displayMode() == EXTEND_MODE) {
        
        if(m_monitor->isPrimary()) {
            QPen penWhite(Qt::white);
            const qreal width = r.width() / 2.0;
            const qreal height = r.height() / 2.0;
            painter->setBrush(Qt::white);
            painter->drawRoundedRect(QRectF(r.x() + r.width() / 4.0, r.y() + r.height() * 9.0 / 10.0,width, height), radius, radius);
        }

        //根据是否焦点绘制选中状态
        if (m_selected) {
            // draw blue border if the mode is EXTEND_MODE
            QPen penWhite(QColor("#2ca7f8"));
            penWhite.setWidthF(radius/5.0);
            painter->setPen(penWhite);
            painter->setBrush(Qt::transparent);
            r.adjust(penWhite.width() /2, penWhite.width() / 2, -penWhite.width() /2, -penWhite.width() / 2);
            painter->drawRoundedRect(r, radius, radius);
        }
    }
    
    painter->restore();
}

void MonitorProxyWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_model->displayMode() == EXTEND_MODE) {
        Q_EMIT requestMonitorPress(m_monitor);
    }
    QGraphicsItem::mousePressEvent(event);
}

void MonitorProxyWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);

    if (m_model->displayMode() == EXTEND_MODE) {
        m_isMoving = true;
        Q_EMIT requestMouseMove(this);
    }
}

void MonitorProxyWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_model->displayMode() == EXTEND_MODE) {

        if(m_isMoving) {
            Q_EMIT requestMonitorRelease(m_monitor);
            m_isMoving = false;
        }
    }

    QGraphicsItem::mouseReleaseEvent(event);
}

void MonitorProxyWidget::focusInEvent(QFocusEvent *event)
{
    //增加选中的效果
    m_selected = true;

    QGraphicsItem::focusInEvent(event);
}
void MonitorProxyWidget::focusOutEvent(QFocusEvent *event) 
{
    //增加选中的效果
    m_selected = false;

    QGraphicsItem::focusOutEvent(event);
}

void MonitorProxyWidget::keyPressEvent(QKeyEvent *event)
{
    QGraphicsItem::keyPressEvent(event);

   if (m_model->displayMode() == EXTEND_MODE) {
        Q_EMIT requestKeyPress(this, event->key());
   }
}

QRectF MonitorProxyWidget::boundingRect() const
{
    return QRectF(0, 0, this->w(), this->h());
}

QRectF MonitorProxyWidget::bufferboundingRect() const
{
    return boundingRect().adjusted(-200, -200, 200, 200);
}

QRectF MonitorProxyWidget::justIntersectRect() const
{
    return boundingRect().adjusted(10, 10, -10, -10);
}

//外扩0.05个像素, 规避由于计算导致精度丢失或者坐标值完全一致的情况下不能判定为相交的情况
QRectF MonitorProxyWidget::boundingRectEx() const
{
    return boundingRect().adjusted(-0.05, -0.05, 0.05, 0.05);
}
