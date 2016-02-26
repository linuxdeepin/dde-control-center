/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DTIPSFRAME_H
#define DTIPSFRAME_H

#include <QLabel>
#include <QPropertyAnimation>

class DTipsFrame : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(ArrowDirection arrowDirection READ arrowDirection WRITE setArrowDirection)

    Q_ENUMS(ArrowDirection)
public:
    enum ArrowDirection {
        ArrowLeft,
        ArrowRight,
        ArrowTop,
        ArrowBottom
    };

    DTipsFrame();
    ~DTipsFrame();

    void move(int x, int y);
    ArrowDirection arrowDirection() const;

private:
    void updateStyle();
public slots:
    void followTheSender();
    void showTipsTextAt(const QString &text, const QPoint &p);
    void show();
    void setArrowDirection(ArrowDirection arrowDirection);

private:
    QLabel *m_label;

    QPropertyAnimation *m_moveAni;

    bool m_isFirstMove = true;

    ArrowDirection m_arrowDirection = ArrowRight;
};

#endif // DTIPSFRAME_H
