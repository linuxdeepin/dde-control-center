//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#ifndef MONITORPROXYWIDGET_H
#define MONITORPROXYWIDGET_H

#include "interface/namespace.h"

#include <QAbstractGraphicsShapeItem>

class QScrollArea;
class QKeyEvent;
class QPainter;
class QGraphicsSceneMouseEvent;

namespace DCC_NAMESPACE {
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
    inline bool getReSplicing() { return m_reSplicing; }
    inline void setReSplicing(bool value) { m_reSplicing = value; }

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
    inline QRectF justIntersectRectLeft() const { return QRectF(QPointF(boundingRectEx().left(), justIntersectRect().top()), QPointF(justIntersectRect().left(), justIntersectRect().bottom())); }
    inline QRectF justIntersectRectRight() const { return QRectF(QPointF(justIntersectRect().right(), justIntersectRect().top()), QPointF(boundingRectEx().right(), justIntersectRect().bottom())); }
    inline QRectF justIntersectRectBottom() const { return QRectF(QPointF(justIntersectRect().left(), justIntersectRect().bottom()), QPointF(justIntersectRect().right(), boundingRectEx().bottom())); }
    inline QRectF justIntersectRectTop() const { return QRectF(QPointF(justIntersectRect().left(), boundingRectEx().top()), QPointF(justIntersectRect().right(), justIntersectRect().top())); }

    //顶点 左上-左下-右上-右下
    inline QRectF justIntersectRectLeftTop() const { return QRectF(QPointF(boundingRectEx().left(), boundingRectEx().top()), QPointF(justIntersectRect().left(), justIntersectRect().top())); }
    inline QRectF justIntersectRectLeftBottom() const { return QRectF(QPointF(boundingRectEx().left(), justIntersectRect().bottom()), QPointF(justIntersectRect().left(), boundingRectEx().bottom())); }
    inline QRectF justIntersectRectRightTop() const { return QRectF(QPointF(justIntersectRect().right(), boundingRectEx().top()), QPointF(boundingRectEx().right(), justIntersectRect().top())); }
    inline QRectF justIntersectRectRightBottom() const { return QRectF(QPointF(justIntersectRect().right(), justIntersectRect().bottom()), QPointF(boundingRectEx().right(), boundingRectEx().bottom())); }
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

    bool m_reSplicing; //重新拼接
};
}

#endif // MONITORPROXYWIDGET_H
