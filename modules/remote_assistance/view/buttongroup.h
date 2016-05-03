/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef BUTTONGROUP_H
#define BUTTONGROUP_H

#include <QFrame>

#include <dwidget_global.h>

DWIDGET_BEGIN_NAMESPACE
class DHeaderLine;
DWIDGET_END_NAMESPACE

class ButtonGroup : public QWidget
{
    Q_OBJECT
public:
    ButtonGroup();
    ButtonGroup(const QString &title);

    ButtonGroup *addItem(QWidget *);
    ButtonGroup *addSeparator();
    ButtonGroup *setGroupTitle(const QString &title);

private:
    DTK_WIDGET_NAMESPACE::DHeaderLine* m_header;
};

#endif // BUTTONGROUP_H
