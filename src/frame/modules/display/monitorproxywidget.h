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

    void setMovedX(const int x);
    void setMovedY(const int y);

    const QString name() const;

    inline QPointF getPreCenter() { return m_preCenter; }
    inline void setPreCenter(QPointF p) { m_preCenter = p; }

    QRectF boundingRect() const override;
    QRectF bufferboundingRect() const;
    QRectF justIntersectRect() const;
    QRectF boundingRectEx() const;

    //自动吸附匹配使用
    //四边区域 左-右-下-上
    inline QRectF bufferboundingRectLeft() const { return QRectF(QPointF(bufferboundingRect().left(), boundingRect().top()), QPointF(boundingRect().left(), boundingRect().bottom())); }
    inline QRectF bufferboundingRectRight() const { return QRectF(QPointF(boundingRect().right(), boundingRect().top()), QPointF(bufferboundingRect().right(), boundingRect().bottom())); }
    inline QRectF bufferboundingRectBottom() const { return QRectF(QPointF(boundingRect().left(), boundingRect().bottom()), QPointF(boundingRect().right(), bufferboundingRect().bottom())); }
    inline QRectF bufferboundingRectTop() const { return QRectF(QPointF(boundingRect().left(), bufferboundingRect().top()), QPointF(boundingRect().right(), boundingRect().top())); }

    //顶点 左上-左下-右上-右下
    inline QRectF bufferboundingRectLeftTop() const { return QRectF(QPointF(bufferboundingRect().left(), bufferboundingRect().top()), QPointF(boundingRect().left(), boundingRect().top())); }
    inline QRectF bufferboundingRectLeftBottom() const { return QRectF(QPointF(bufferboundingRect().left(), boundingRect().bottom()), QPointF(boundingRect().left(), bufferboundingRect().bottom())); }
    inline QRectF bufferboundingRectRightTop() const { return QRectF(QPointF(boundingRect().right(), bufferboundingRect().top()), QPointF(bufferboundingRect().right(), boundingRect().top())); }
    inline QRectF bufferboundingRectRightBottom() const { return QRectF(QPointF(boundingRect().right(), boundingRect().bottom()), QPointF(bufferboundingRect().right(), bufferboundingRect().bottom())); }


    //刚好相交的边界描述
    //四边区域 左-右-下-上
    inline QRectF justIntersectRectLeft() const { return QRectF(QPointF(boundingRect().left(), justIntersectRect().top()), QPointF(justIntersectRect().left(), justIntersectRect().bottom())); }
    inline QRectF justIntersectRectRight() const { return QRectF(QPointF(justIntersectRect().right(), justIntersectRect().top()), QPointF(boundingRect().right(), justIntersectRect().bottom())); }
    inline QRectF justIntersectRectBottom() const { return QRectF(QPointF(justIntersectRect().left(), justIntersectRect().bottom()), QPointF(justIntersectRect().right(), boundingRect().bottom())); }
    inline QRectF justIntersectRectTop() const { return QRectF(QPointF(justIntersectRect().left(), boundingRect().top()), QPointF(justIntersectRect().right(), justIntersectRect().top())); }

    //顶点 左上-左下-右上-右下
    inline QRectF justIntersectRectLeftTop() const { return QRectF(QPointF(boundingRect().adjusted(-1,-1,1,1).left(), boundingRect().adjusted(-1,-1,1,1).top()), QPointF(justIntersectRect().left(), justIntersectRect().top())); }
    inline QRectF justIntersectRectLeftBottom() const { return QRectF(QPointF(boundingRect().adjusted(-1,-1,1,1).left(), justIntersectRect().bottom()), QPointF(justIntersectRect().left(), boundingRect().adjusted(-1,-1,1,1).bottom())); }
    inline QRectF justIntersectRectRightTop() const { return QRectF(QPointF(justIntersectRect().right(), boundingRect().adjusted(-1,-1,1,1).top()), QPointF(boundingRect().adjusted(-1,-1,1,1).right(), justIntersectRect().top())); }
    inline QRectF justIntersectRectRightBottom() const { return QRectF(QPointF(justIntersectRect().right(), justIntersectRect().bottom()), QPointF(boundingRect().adjusted(-1,-1,1,1).right(), boundingRect().adjusted(-1,-1,1,1).bottom())); }

    inline void updateScale(const qreal scale) { m_scale = scale; }

Q_SIGNALS:
    void requestMonitorPress(Monitor *mon);
    void requestMonitorRelease(Monitor *mon);
    void requestMouseMove(MonitorProxyWidget *self) const;
    void requestKeyPress(MonitorProxyWidget *self, int keyValue) const;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Monitor *m_monitor;
    DisplayModel *m_model;

    int m_movedX;
    int m_movedY;

    QPointF m_preCenter; //记录上一次调整的中心点

    bool m_selected;
    bool m_isMoving; //表示item被移动了

    qreal m_scale;
};

} // namespace display

} // namespace dcc

#endif // MONITORPROXYWIDGET_H
