/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QPropertyAnimation>

#include "sidebar.h"
#include "constants.h"
#include "sidebarmodel.h"
#include "sidebardelegate.h"

SideBar::SideBar(QWidget *parent)
    : QFrame(parent)
{
    m_sidebarView = new SidebarView;
    m_sidebarView->setModel(new SidebarModel);
    m_sidebarView->setItemDelegate(new SidebarDelegate);

    m_topScrollHandle = new ScrollHandle;
    m_topScrollHandle->setFixedSize(16, 24);

    m_botScrollHandle = new ScrollHandle;
    m_botScrollHandle->setFixedSize(16, 24);

    m_sidebarLayout = new QVBoxLayout;
    m_sidebarLayout->addWidget(m_topScrollHandle);
    m_sidebarLayout->setAlignment(m_topScrollHandle, Qt::AlignHCenter);
    m_sidebarLayout->addWidget(m_sidebarView, 1);
    m_sidebarLayout->setAlignment(m_sidebarView, Qt::AlignHCenter);
    m_sidebarLayout->addWidget(m_botScrollHandle);
    m_sidebarLayout->setAlignment(m_botScrollHandle, Qt::AlignHCenter);
    m_sidebarLayout->addStretch();
    m_sidebarLayout->setContentsMargins(0, 10, 0, 0);
    m_sidebarLayout->setSpacing(0);

    connect(m_sidebarView, &SidebarView::moduleSelected, this, &SideBar::moduleSelected);
    connect(m_sidebarView, &SidebarView::scrolled, this, &SideBar::adjustScrollBox, Qt::QueuedConnection);
    connect(m_sidebarView, &SidebarView::sizeChanged, this, &SideBar::adjustScrollBox, Qt::QueuedConnection);
    connect(m_topScrollHandle, &ScrollHandle::triggerd, m_sidebarView, &SidebarView::scrollUp);
    connect(m_botScrollHandle, &ScrollHandle::triggerd, m_sidebarView, &SidebarView::scrollDown);

    setFixedWidth(DCC::SideBarWidth);
    setLayout(m_sidebarLayout);
}

void SideBar::switchToModule(const ModuleMetaData &meta)
{
    SidebarModel *model = qobject_cast<SidebarModel *>(m_sidebarView->model());
    model->setSelectedPluginId(meta.id);
    m_sidebarView->scrollToItem(meta.name);
}

void SideBar::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);

    m_sidebarView->setMaximumSize(m_sidebarView->sizeHint());
}

void SideBar::adjustScrollBox()
{
    m_topScrollHandle->setPixmap(QPixmap(!m_sidebarView->firstItemVisible() ? ":/resources/images/arrow_up_normal.png" : ""));
    m_botScrollHandle->setPixmap(QPixmap(!m_sidebarView->lastItemVisible() ? ":/resources/images/arrow_down_normal.png" : ""));
}

ScrollHandle::ScrollHandle(QWidget *parent)
    : QLabel(parent)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(400);

    connect(m_timer, &QTimer::timeout, this, &ScrollHandle::triggerd);
}

void ScrollHandle::enterEvent(QEvent *e)
{
    QLabel::enterEvent(e);

    m_timer->start(200);
}

void ScrollHandle::leaveEvent(QEvent *e)
{
    QLabel::leaveEvent(e);

    m_timer->stop();
}
