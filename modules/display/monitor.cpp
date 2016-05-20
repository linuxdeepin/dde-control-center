/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QDebug>
#include <QTimer>

#include <dwidget_global.h>
#include <dthememanager.h>

#include "monitor.h"
#include "dbus/monitorinterface.h"

DWIDGET_USE_NAMESPACE

Monitor::Monitor(MonitorInterface *dbus, QWidget *parent) :
    QFrame(parent),
    m_dbusInterface(dbus),
    m_draggable(false),
    m_nameAlignment(Qt::AlignCenter),
    m_draging(false),
    m_eyeing(false),
    m_isPrimary(false),
    m_child(NULL)
{
    D_THEME_INIT_WIDGET(Monitor, draging, eyeing);

    if(dbus){
        m_resolution = QRect(m_dbusInterface->x(), m_dbusInterface->y(), m_dbusInterface->width(), m_dbusInterface->height());

        connect(m_dbusInterface, &MonitorInterface::XChanged, [this] {
            m_resolution.moveLeft(m_dbusInterface->x());
            emit resolutionChanged(m_resolution);
        });
        connect(m_dbusInterface, &MonitorInterface::YChanged, [this] {
            m_resolution.moveTop(m_dbusInterface->y());
            emit resolutionChanged(m_resolution);
        });
        connect(m_dbusInterface, &MonitorInterface::WidthChanged, [this] {
            m_resolution.setWidth(m_dbusInterface->width());
            emit resolutionChanged(m_resolution);
        });
        connect(m_dbusInterface, &MonitorInterface::HeightChanged, [this] {
            m_resolution.setHeight(m_dbusInterface->height());
            emit resolutionChanged(m_resolution);
        });
        connect(m_dbusInterface, &MonitorInterface::RotationChanged, [this] {
            m_resolution = QRect(m_dbusInterface->x(), m_dbusInterface->y(), m_dbusInterface->width(), m_dbusInterface->height());
            emit resolutionChanged(m_resolution);
        });
    }

    connect(this, SIGNAL(resolutionChanged(QRect)), SLOT(update()));
}


void Monitor::setName(QString name)
{
    if(m_name == name)
        return;

    m_name = name;

    update();
}

QRect Monitor::resolution() const
{
    return m_resolution;
}

MonitorInterface *Monitor::dbusInterface() const
{
    return m_dbusInterface;
}

QColor Monitor::dockBgColor() const
{
    return m_dockBgColor;
}

QColor Monitor::childBorderColor() const
{
    return m_childBorderColor;
}

bool Monitor::draggable() const
{
    return m_draggable;
}

QString Monitor::name() const
{
    return m_name;
}

Qt::Alignment Monitor::nameAlignment() const
{
    return m_nameAlignment;
}

bool Monitor::draging() const
{
    return m_draging;
}

bool Monitor::eyeing() const
{
    return m_eyeing;
}

bool Monitor::isPrimary() const
{
    return m_isPrimary;
}

bool Monitor::hasChanged() const
{
    return pos() != m_oldPos;
}

const Monitor *Monitor::child() const
{
    return m_child;
}

QRect Monitor::parentRect() const
{
    return m_parentRect;
}

void Monitor::setParentRect(const QRect &rect)
{
    m_parentRect = rect;
}

QPoint Monitor::mapToRealPoint() const
{
    return QPoint((x() - parentRect().x()) * resolution().width() / width(),
                  (y() - parentRect().y()) * resolution().width() / width());
}

void Monitor::setResolution(const QRect &rect)
{
    m_resolution = rect;
}

void Monitor::setDockBgColor(QColor dockBgColor)
{
    if(dockBgColor == m_dockBgColor)
        return;

    m_dockBgColor = dockBgColor;

    update();
}

void Monitor::setChildBorderColor(QColor childBorderColor)
{
    if(m_childBorderColor == childBorderColor)
        return;

    m_childBorderColor = childBorderColor;

    update();
}

void Monitor::setDraggable(bool draggable)
{
    if(m_draggable == draggable)
        return;

    m_draggable = draggable;

    if(!draggable)
        parentWidget()->setFocus();
}

void Monitor::setAlignment(Qt::Alignment aalignment)
{
    m_nameAlignment = aalignment;
}

void Monitor::setNameAlignment(Qt::Alignment nameAlignment)
{
    m_nameAlignment = nameAlignment;
}

void Monitor::setIsPrimary(bool primary)
{
    if(m_isPrimary == primary)
        return;

    m_isPrimary = primary;

    update();
}

void Monitor::paintEvent(QPaintEvent *e)
{
    QFrame::paintEvent(e);

    QPainter pa;
    pa.begin(this);

    QRect child_rect;

    if(m_child){
        child_rect.setWidth(width() * 0.5625);
        child_rect.setHeight(height() * 0.4);
        child_rect.moveTo(width() / 8, height() / 4);

        QRect me_rect = child_rect;
        me_rect.moveTopLeft(QPoint(child_rect.right() - child_rect.width() * 0.667,
                                   child_rect.bottom() - child_rect.height() * 0.667));

        QTextOption tmpOption;
        QFont font = pa.font();

        tmpOption.setAlignment(Qt::AlignRight);
        font.setPixelSize(child_rect.height() / 3.5);
        pa.setFont(font);
        QRect tmp_rect = rect();
        tmp_rect.setRight(tmp_rect.right() - tmp_rect.height() / 15.0);
        tmp_rect.setTop(tmp_rect.top() + tmp_rect.height() / 15.0);
        pa.drawText(tmp_rect, m_dbusInterface->name(), tmpOption);

        tmpOption.setAlignment(Qt::AlignCenter);
        font.setPixelSize(child_rect.height() / 1.3);

        pa.fillRect(child_rect, Qt::black);
        pa.setFont(font);
        pa.drawText(child_rect, "A", tmpOption);
        QPen tmp_pen = pa.pen();
        pa.setPen(m_childBorderColor);
        pa.drawRect(child_rect);

        pa.setOpacity(0.85);
        pa.fillRect(me_rect, Qt::black);
        pa.setOpacity(1);
        pa.drawRect(me_rect);
        pa.setPen(tmp_pen);
        pa.drawText(me_rect, "A", tmpOption);

        font.setPixelSize(12);
        pa.setFont(font);
        tmpOption.setAlignment(Qt::AlignRight|Qt::AlignTop);

        pa.drawText(child_rect, m_child->name(), tmpOption);
        pa.drawText(me_rect, m_name, tmpOption);
    }else{
        int top_width = width() / 3.0;
        int height = top_width / 8.0;

        QTextOption nameOption;
        nameOption.setAlignment(m_nameAlignment);
        pa.drawText(rect(), m_name, nameOption);

        if(m_draging || hasFocus()){
            const QPoint point = m_draging && hasChanged() ? mapToRealPoint() : resolution().topLeft();

            pa.drawText(10, 20, QString("(%1,%2)").arg(point.x()).arg(point.y()));
        }

        if(m_isPrimary){
            QPainterPath path;
            path.moveTo(top_width, this->height() - height);
            path.lineTo(top_width * 2, this->height() - height);
            path.lineTo(top_width * 2.1, this->height());
            path.lineTo(top_width * 0.9, this->height());
            path.lineTo(top_width, this->height() - height);

            pa.setRenderHint(QPainter::Antialiasing);
            pa.fillPath(path, m_dockBgColor);
        }
    }

    pa.end();
}

void Monitor::mousePressEvent(QMouseEvent *e)
{
    QFrame::mousePressEvent(e);

    if(!m_draggable){
        e->ignore();
        return;
    }

    m_pressPos = e->pos();
    m_oldPos = pos();

    setCursor(Qt::ClosedHandCursor);
    setFocus();
    setDraging(true);

    emit mousePressed(e->pos());
}

void Monitor::mouseMoveEvent(QMouseEvent *e)
{
    QFrame::mouseMoveEvent(e);

    if(!m_draging){
        e->ignore();
        return;
    }

    move(pos() + e->pos() - m_pressPos);

    emit mouseMoveing(e->pos());
}

void Monitor::mouseReleaseEvent(QMouseEvent *e)
{
    QFrame::mouseReleaseEvent(e);

    if(!m_draging){
        e->ignore();
        return;
    }

    setCursor(Qt::ArrowCursor);

    setDraging(false);

    emit mouseRelease(e->pos());
}

void Monitor::keyPressEvent(QKeyEvent *e)
{
    e->accept();

    switch (e->key()) {
    case Qt::Key_Left:
        m_resolution.moveLeft(m_resolution.x() - 1);
        emit resolutionChanged(m_resolution);
        break;
    case Qt::Key_Right:
        m_resolution.moveLeft(m_resolution.x() + 1);
        emit resolutionChanged(m_resolution);
        break;
    case Qt::Key_Up:
        m_resolution.moveTop(m_resolution.y() - 1);
        emit resolutionChanged(m_resolution);
        break;
    case Qt::Key_Down:
        m_resolution.moveTop(m_resolution.y() + 1);
        emit resolutionChanged(m_resolution);
        break;
    default:
        break;
    }
}

bool Monitor::dragEnter(Monitor *monitor)
{
    if(m_child || monitor == this)
        return false;

    setEyeing(true);

    return true;
}

void Monitor::dragLeave()
{
    setEyeing(false);
}

bool Monitor::drop(Monitor *monitor)
{
    if(m_child || monitor == this)
        return false;

    setEyeing(false);

    m_child = monitor;
    m_child->hide();

    update();

    return true;
}

Monitor *Monitor::split()
{
    Monitor *m = m_child;
    m_child = NULL;

    update();

    return m;
}

void Monitor::applyPostion()
{
    m_dbusInterface->SetPos(m_resolution.left(), m_resolution.top()).waitForFinished();
}

void Monitor::resetResolution()
{
    m_resolution = QRect(m_dbusInterface->x(), m_dbusInterface->y(), m_dbusInterface->width(), m_dbusInterface->height());
}

void Monitor::setDraging(bool arg)
{
    if(m_draging == arg)
        return;

    m_draging = arg;

    update();

    emit dragingChanged(arg);
}

void Monitor::setEyeing(bool arg)
{
    if(m_eyeing == arg)
        return;

    m_eyeing = arg;

    emit eyeingChanged(arg);
}
