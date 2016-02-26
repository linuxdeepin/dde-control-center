/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef ABSTRACTVIEW_H
#define ABSTRACTVIEW_H

#include <QFrame>
#include <libdui_global.h>

class QVBoxLayout;
class QHBoxLayout;

DUI_BEGIN_NAMESPACE
class DTextButton;
DUI_END_NAMESPACE

class AbstractView : public QWidget
{
    Q_OBJECT
public:
    AbstractView(QWidget *p = nullptr);
    AbstractView *addWidget(QWidget *);
    AbstractView *addSpacing(int);
    AbstractView *addLayout(QLayout *, int = 0);
    AbstractView *addButton(DUI_NAMESPACE::DTextButton *btn, int stretch = 0, Qt::Alignment alignment = 0);

protected:
    QWidget *m_mainWidget;
    QVBoxLayout *m_viewLayout;
    QHBoxLayout *m_buttonLayout;

    virtual void initialize();
    virtual QWidget *createMainWidget() = 0;
};

#endif // ABSTRACTVIEW_H
