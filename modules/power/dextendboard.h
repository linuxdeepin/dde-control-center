/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DEXTENDBOARD_H
#define DEXTENDBOARD_H

#include <QtCore/QStringList>
#include <QtCore/QPropertyAnimation>
#include <QtPlugin>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtGui/QResizeEvent>
#include <dbuttongrid.h>
#include <dseparatorhorizontal.h>

#include "constants.h"
#include "trianglerectangle.h"

DWIDGET_USE_NAMESPACE
class DExtendBoard : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(int height READ height WRITE setHeight)
public:
    DExtendBoard(QFrame *parent = 0);
    ~DExtendBoard();

    void setExpand(bool expand);

    void setWidth(int ex_width);
    void setHeight(int ex_height);
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    DButtonGrid *m_suspendButtonGrid;
    DButtonGrid *m_idleButtonGrid;

private:
    bool isExpand = false;
    QHBoxLayout *customIdleLayout;
    QHBoxLayout *idleLayout;
    QHBoxLayout *suspendLayout;
    QLabel *m_label;
    TriangleRectangle *m_guideExtendTriangle;
    QVBoxLayout *m_contentLayout;
    QLabel *m_customLabel;
    QLabel *m_customChooseLabel;
    QLabel *m_idleLabel;

    QLabel *m_suspendLabel;
    QStringList m_suspendButtons;
    QStringList m_idleButtons;
    DSeparatorHorizontal *m_bottomSeparator;
    QPropertyAnimation *m_animation = NULL;

};
#endif // DEXTENDBOARD_H

