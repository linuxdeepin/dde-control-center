// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "touchscrollfilter.h"

#include <QEvent>
#include <QTouchEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QApplication>

TouchScrollFilter::TouchScrollFilter(QAbstractScrollArea *area)
    : QObject(area)
    , m_area(area)
    , m_isDrag(false)
{
    if (m_area) {
        m_area->viewport()->setAttribute(Qt::WA_AcceptTouchEvents, true);
        m_area->setAttribute(Qt::WA_AcceptTouchEvents, true);
        m_area->viewport()->installEventFilter(this);
    }
}

bool TouchScrollFilter::eventFilter(QObject *watched, QEvent *event)
{
    if (!m_area)
        return QObject::eventFilter(watched, event);

    const QEvent::Type type = event->type();
    if (type != QEvent::TouchBegin && type != QEvent::TouchUpdate && 
        type != QEvent::TouchEnd && type != QEvent::TouchCancel) {
        return QObject::eventFilter(watched, event);
    }

    QTouchEvent *te = static_cast<QTouchEvent *>(event);
    if (te->points().isEmpty()) {
        event->accept();
        return true;
    }

    QEventPoint pt = te->points().first();

    switch (type) {
    case QEvent::TouchBegin: {
        m_lastPos = pt.globalPosition();
        m_isDrag = false;
        break;
    }
    case QEvent::TouchUpdate: {
        qreal deltaY = pt.globalPosition().y() - m_lastPos.y();
        qreal deltaX = pt.globalPosition().x() - m_lastPos.x();

        if (!m_isDrag && (qAbs(deltaY) >= QApplication::startDragDistance() || qAbs(deltaX) >= QApplication::startDragDistance())) {
            m_isDrag = true;
        }

        if (m_isDrag) {
            if (qAbs(deltaY) > 0 && m_area->verticalScrollBar()) {
                m_area->verticalScrollBar()->setValue(m_area->verticalScrollBar()->value() - deltaY);
            }
            if (qAbs(deltaX) > 0 && m_area->horizontalScrollBar()) {
                m_area->horizontalScrollBar()->setValue(m_area->horizontalScrollBar()->value() - deltaX);
            }
            m_lastPos = pt.globalPosition();
        }
        break;
    }
    case QEvent::TouchEnd: {
        if (!m_isDrag) {
            // Determine the correct target widget for the click
            QWidget *target = m_area->viewport();
            QPoint widgetPos = pt.position().toPoint();
            
            // If it's a QScrollArea, the content widget is usually a child of the viewport
            if (QWidget *child = target->childAt(widgetPos)) {
                target = child;
                widgetPos = target->mapFrom(m_area->viewport(), widgetPos);
            }

            QMouseEvent press(QEvent::MouseButtonPress, widgetPos, pt.globalPosition(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            QCoreApplication::sendEvent(target, &press);
            QMouseEvent release(QEvent::MouseButtonRelease, widgetPos, pt.globalPosition(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            QCoreApplication::sendEvent(target, &release);
        }
        break;
    }
    default:
        break;
    }

    event->accept();
    return true;
}
