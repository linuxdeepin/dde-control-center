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
#include "numbutton.h"
#include <dimagebutton.h>
#include <appicon.h>

class NumButton;
DWIDGET_USE_NAMESPACE
class Viewer : public QWidget
{
    Q_OBJECT
public:
    explicit Viewer(QWidget *parent = 0);
    ~Viewer();

signals:

public slots:
    void setValue(QString appName,QString appIcon,QString summary,QString body,QString time);

protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);

private:
    NumButton *m_appName,*m_summary,*m_body,*m_time;
    DImageButton *m_close;
    AppIcon *m_appIcon;
    QGridLayout *m_mainlayout;
    QHBoxLayout *m_hboxlayout;
};

#endif // VIEWER_H
