/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef VIEWER_H
#define VIEWER_H

#include <QObject>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <dimagebutton.h>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QStyleOption>
#include <QPainter>

class NumButton;
DWIDGET_USE_NAMESPACE
class Viewer : public QWidget
{
    Q_OBJECT
public:
    explicit Viewer(QWidget *parent = 0);
    void setAppName(const QString &s);
    void setAppIcon(const QString &s);
    void setAppSummary(const QString &s);
    void setAppBody(const QString &s);
    void setAppTime(const QString &s);

signals:
    void close();

protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);

private:
    QLabel *m_appName,*m_body,*m_time;
    DImageButton *m_close;
    QLabel      *m_appIcon;
    QGridLayout *m_mainlayout;
    QHBoxLayout *m_hboxlayout;
    QPropertyAnimation *m_anim1;
};

#endif // VIEWER_H
