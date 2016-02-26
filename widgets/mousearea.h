/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef MOUSEAREA_H
#define MOUSEAREA_H

#include <QWidget>
#include <QMouseEvent>

class MouseAreaPrivate;
class MouseArea : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(bool hoverEnabled READ hoverEnabled WRITE setHoverEnabled NOTIFY hoverEnabledChanged)
    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
    Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)
    Q_PROPERTY(bool containsPress READ containsPress NOTIFY containsPressChanged)
    Q_PROPERTY(bool preventStealing READ preventStealing WRITE setPreventStealing NOTIFY preventStealingChanged)
    Q_PROPERTY(QPoint mousePos READ mousePos NOTIFY mousePosChanged)
    Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedButtons NOTIFY acceptedButtonsChanged)
    Q_PROPERTY(Qt::MouseButtons pressedButtons READ pressedButtons NOTIFY pressedButtonsChanged)
    //Q_PROPERTY(DragInfo* drag READ drag WRITE setDrag NOTIFY dragChanged)

public:
    explicit MouseArea(QWidget *parent = 0);

    Qt::MouseButtons pressedButtons() const;
    bool preventStealing() const;
    bool containsPress() const;
    bool containsMouse() const;
    Qt::MouseButtons acceptedButtons() const;
    QPoint mousePos() const;
    bool hoverEnabled() const;
    bool pressed() const;

public slots:
    void setPreventStealing(bool preventStealing);
    void setAcceptedButtons(Qt::MouseButtons acceptedButtons);
    void setHoverEnabled(bool hoverEnabled);

signals:
    void canceled();
    void clicked(QMouseEvent *mouse);
    void doubleClicked(QMouseEvent *mouse);
    void entered();
    void exited();
    void positionChanged(QMouseEvent *mouse);
    void pressAndHold(QMouseEvent *mouse);
    void pressed(QMouseEvent *mouse);
    void released(QMouseEvent *mouse);
    void wheel(QWheelEvent *wheel);
    void hoverEnabledChanged(bool hoverEnabled);
    void pressedChanged(bool pressed);
    void mousePosChanged(QPoint mousePos);
    void acceptedButtonsChanged(Qt::MouseButtons acceptedButtons);
    void cursorShapeChanged(Qt::CursorShape cursorShape);
    void containsMouseChanged(bool containsMouse);
    void containsPressChanged(bool containsPress);
    void preventStealingChanged(bool preventStealing);
    void pressedButtonsChanged(Qt::MouseButtons pressedButtons);

protected:
    explicit MouseArea(MouseAreaPrivate *dd, QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);

private:
    MouseAreaPrivate *d_ptr;

    Q_DECLARE_PRIVATE(MouseArea)
};

#endif // MOUSEAREA_H
