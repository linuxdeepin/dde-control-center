/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QMouseEvent>

#include "mousearea.h"

class MouseAreaPrivate
{
    MouseAreaPrivate(MouseArea *qq) : q_ptr(qq){}

    MouseArea *q_ptr;

    bool m_hoverEnabled = false;
    bool m_pressed = false;
    bool m_containsMouse = false;
    bool m_containsPress = false;
    bool m_preventStealing = false;
    QPoint m_mousePos;
    Qt::MouseButtons m_acceptedButtons = Qt::LeftButton;
    Qt::MouseButtons m_pressedButtons = Qt::NoButton;

    Q_DECLARE_PUBLIC(MouseArea)

    void setPressed(bool pressed)
    {
        Q_Q(MouseArea);

        if (m_pressed == pressed) {
            return;
        }

        if(pressed)
            setContainsMouse(true);

        m_pressed = pressed;
        emit q->pressedChanged(pressed);
    }

    void setPressedButtons(Qt::MouseButtons pressedButtons)
    {
        Q_Q(MouseArea);

        if (m_pressedButtons == pressedButtons) {
            return;
        }

        m_pressedButtons = pressedButtons;
        emit q->pressedButtonsChanged(pressedButtons);
    }

    void setContainsMouse(bool containsMouse)
    {
        Q_Q(MouseArea);

        if (m_containsMouse == containsMouse) {
            return;
        }

        m_containsMouse = containsMouse;

        setContainsMouse(containsMouse && q->pressed());

        emit q->containsMouseChanged(containsMouse);
    }

    void setContainsPress(bool containsPress)
    {
        Q_Q(MouseArea);

        if (m_containsPress == containsPress) {
            return;
        }

        m_containsPress = containsPress;
        emit q->containsPressChanged(containsPress);
    }

    void setMousePos(QPoint mousePos)
    {
        Q_Q(MouseArea);

        if (m_mousePos == mousePos) {
            return;
        }

        m_mousePos = mousePos;
        emit q->mousePosChanged(mousePos);
    }
};

MouseArea::MouseArea(QWidget *parent) :
    QWidget(parent),
    d_ptr(new MouseAreaPrivate(this))
{
    setMouseTracking(false);
}

MouseArea::MouseArea(MouseAreaPrivate *dd, QWidget *parent) :
    QWidget(parent),
    d_ptr(dd)
{
    setMouseTracking(false);
}

void MouseArea::mousePressEvent(QMouseEvent *e)
{
    Q_D(MouseArea);

    QWidget::mousePressEvent(e);
    d->setMousePos(e->pos());
    d->setPressed(true);
}

void MouseArea::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(MouseArea);

    QWidget::mouseReleaseEvent(e);
    d->setMousePos(e->pos());
    d->setPressed(false);
    emit clicked(e);
}

void MouseArea::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(MouseArea);

    QWidget::mouseReleaseEvent(e);
    d->setMousePos(e->pos());
    emit mousePosChanged(e->pos());
    emit positionChanged(e);
}

void MouseArea::enterEvent(QEvent *e)
{
    Q_D(MouseArea);

    QWidget::enterEvent(e);
    d->setContainsMouse(true);
    emit entered();
}

void MouseArea::leaveEvent(QEvent *e)
{
    Q_D(MouseArea);

    QWidget::leaveEvent(e);
    d->setContainsMouse(false);
    emit exited();
}

bool MouseArea::hoverEnabled() const
{
    Q_D(const MouseArea);

    return d->m_hoverEnabled;
}

bool MouseArea::pressed() const
{
    Q_D(const MouseArea);

    return d->m_pressed;
}

QPoint MouseArea::mousePos() const
{
    Q_D(const MouseArea);

    return d->m_mousePos;
}

Qt::MouseButtons MouseArea::acceptedButtons() const
{
    Q_D(const MouseArea);

    return d->m_acceptedButtons;
}

bool MouseArea::containsMouse() const
{
    Q_D(const MouseArea);

    return d->m_containsMouse;
}

bool MouseArea::containsPress() const
{
    Q_D(const MouseArea);

    return d->m_containsPress;
}

bool MouseArea::preventStealing() const
{
    Q_D(const MouseArea);

    return d->m_preventStealing;
}

Qt::MouseButtons MouseArea::pressedButtons() const
{
    Q_D(const MouseArea);

    return d->m_pressedButtons;
}

void MouseArea::setHoverEnabled(bool hoverEnabled)
{
    Q_D(MouseArea);

    if (d->m_hoverEnabled == hoverEnabled) {
        return;
    }
    setMouseTracking(hoverEnabled);
    d->m_hoverEnabled = hoverEnabled;
    emit hoverEnabledChanged(hoverEnabled);
}

void MouseArea::setAcceptedButtons(Qt::MouseButtons acceptedButtons)
{
    Q_D(MouseArea);

    if (d->m_acceptedButtons == acceptedButtons) {
        return;
    }

    d->m_acceptedButtons = acceptedButtons;
    emit acceptedButtonsChanged(acceptedButtons);
}

void MouseArea::setPreventStealing(bool preventStealing)
{
    Q_D(MouseArea);

    if (d->m_preventStealing == preventStealing) {
        return;
    }

    d->m_preventStealing = preventStealing;
    emit preventStealingChanged(preventStealing);
}
