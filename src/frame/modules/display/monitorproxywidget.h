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

#ifndef MONITORPROXYWIDGET_H
#define MONITORPROXYWIDGET_H

#include <QWidget>
#include <QAbstractGraphicsShapeItem>
#include <QGraphicsSceneMouseEvent>

class QScrollArea;

namespace dcc {

namespace display {

class DisplayModel;
class Monitor;
class MonitorProxyWidget : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT
public:
    explicit MonitorProxyWidget(Monitor *mon, DisplayModel *model);

    inline int x() const { return m_movedX; }
    inline int y() const { return m_movedY; }
    int w() const;
    int h() const;

    inline void setMovedX(const int x) { m_movedX = x; }
    inline void setMovedY(const int y) { m_movedY = y; }

    const QString name() const;

    inline QPointF getCenter() { return m_center; }
    inline void setCenter(QPointF p) { m_center = p; }

    inline QPointF getEdge() { return m_edge; }
    inline void setEdge(QPointF p) { m_edge = p; }

    QRectF boundingRect() const override;
    QRectF bufferboundingRect() const;
    QRectF adsorptionbufferboundingRect() const;


Q_SIGNALS:
    void requestApplyMove(MonitorProxyWidget *self) const;
    void requestMonitorPress(Monitor *mon);
    void requestMonitorRelease(Monitor *mon);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Monitor *m_monitor;
    DisplayModel *m_model;

    int m_movedX;
    int m_movedY;

    QPointF m_center;
    QPointF m_edge;
};

} // namespace display

} // namespace dcc

#endif // MONITORPROXYWIDGET_H
