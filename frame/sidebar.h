/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QFrame>
#include <QAbstractButton>
#include <QVBoxLayout>
#include <QTimer>

#include "modulemetadata.h"
#include "dtipsframe.h"
#include "pluginsmanager.h"
#include "sidebarview.h"

class ScrollHandle;
class SideBar : public QFrame
{
    Q_OBJECT
public:
    explicit SideBar(QWidget *parent = 0);

    void switchToModule(const ModuleMetaData &pluginId);

signals:
    void moduleSelected(ModuleMetaData meta);

protected:
    void resizeEvent(QResizeEvent *e);

private:
    void adjustScrollBox();

private:
    QVBoxLayout *m_sidebarLayout;
    SidebarView *m_sidebarView;
    ScrollHandle *m_topScrollHandle;
    ScrollHandle *m_botScrollHandle;
};

class ScrollHandle : public QLabel
{
    Q_OBJECT

public:
    explicit ScrollHandle(QWidget *parent = 0);

signals:
    void triggerd() const;

protected:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);

private:
    QTimer *m_timer;
};

#endif // sidebar.h
