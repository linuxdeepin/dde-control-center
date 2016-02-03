/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DSHINELABEL
#define DSHINELABEL
#include <QtWidgets/QLabel>
#include <QtGui/QPalette>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QTimer>

//The label can just control the fade in or out of text

class DBreathingLabel: public QLabel
{
    Q_OBJECT
    Q_PROPERTY(int alpha READ alpha WRITE setAlpha)
public:
    DBreathingLabel(QWidget *parent=0, Qt::WindowFlags f = 0);
    DBreathingLabel(const QString & text, QWidget * parent = 0,
                    Qt::WindowFlags f = 0);
    ~DBreathingLabel();
    void setColor(QColor color);

    int alpha() const;
public slots:
    void showLabel();
    void hideLabel();
    void setDuration(int duration);
private:
    int alpha_;

    void setAlpha(int alpha);
    QPropertyAnimation* m_showAnimation;
    QPropertyAnimation* m_hideAnimation;
    QPalette m_palette;
};
#endif // DSHINELABEL

