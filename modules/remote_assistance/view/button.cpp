/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QString>
#include <QLabel>
#include <QStackedLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QMouseEvent>
#include <QDebug>
#include <QCoreApplication>

#include <dthememanager.h>
#include <dimagebutton.h>

#include "constants.h"
#include "mousearea.h"

#include "../helper.h"
#include "button.h"

static const int button_fixed_height = 70;

DWIDGET_USE_NAMESPACE

Button::Button(const QString& title, const QString& tip, QWidget* p)
    : QFrame(p),
      m_hlayout(new QHBoxLayout),
      m_vlayout(new QVBoxLayout),
      m_icon(new DImageButton),
      m_title(new QLabel),
      m_tip(new QLabel),
      m_arrow(new DImageButton)
{
    setObjectName("display");
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setFixedHeight(button_fixed_height);

    auto contentWrap = new QWidget(this);
    contentWrap->setFixedWidth(DCC::ModuleContentWidth);

    // To make sure mousearea stacked above the real content, mousearea MUST add to this after real content.
    auto mousearea = new MouseArea(this);
    mousearea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mousearea->setFixedHeight(button_fixed_height);
    mousearea->setFixedWidth(DCC::ModuleContentWidth);
    connect(mousearea, SIGNAL(pressed(QMouseEvent*)), this, SLOT(onMousePressed(QMouseEvent*)));
    connect(mousearea, SIGNAL(released(QMouseEvent*)), this, SLOT(onMouseReleased(QMouseEvent*)));

    m_hlayout->setSpacing(0);
    m_hlayout->setMargin(0);

    m_icon->setAlignment(Qt::AlignVCenter);
    m_icon->setObjectName("icon");
    m_icon->setFixedHeight(button_fixed_height);
    m_hlayout->addWidget(m_icon);

    m_title->setAlignment(Qt::AlignTop);
    m_title->setText(title);
    m_title->setObjectName("title");

    m_tip->setObjectName("tip");
    m_tip->setAlignment(Qt::AlignTop);
    m_tip->setWordWrap(true);
    m_tip->setText(tip);

    m_vlayout->setSpacing(0);
    m_vlayout->setMargin(0);
    m_vlayout->addWidget(m_title);
    m_vlayout->addWidget(m_tip);

    m_hlayout->addLayout(m_vlayout);
    m_hlayout->addStretch();

    m_arrow->setObjectName("arrow");
    m_arrow->setAlignment(Qt::AlignVCenter);
    m_arrow->setNormalPic(getThemeImage("right-arrow-normal.png"));
    m_arrow->setHoverPic(getThemeImage("right-arrow-hover.png"));
    m_arrow->setPressPic(getThemeImage("right-arrow-press.png"));
    m_arrow->setFixedHeight(button_fixed_height);
    m_hlayout->addWidget(m_arrow);

    contentWrap->setLayout(m_hlayout);
    QString style = readStyleSheet("button");
    setStyleSheet(style);
}


void Button::setTitle(const QString& title)
{
    m_title->setText(title);
}

void Button::setTip(const QString& tip)
{
    m_tip->setText(tip);
}

void Button::setNormalIcon(const QString &icon)
{
    m_icon->setNormalPic(icon);
}

void Button::setHoverIcon(const QString &icon)
{
    m_icon->setHoverPic(icon);
}

void Button::setPressedIcon(const QString &icon)
{
    m_icon->setPressPic(icon);
}

void Button::enterEvent(QEvent* e)
{
    QCoreApplication::sendEvent(m_icon, e);
    QCoreApplication::sendEvent(m_arrow, e);
}


void Button::leaveEvent(QEvent* e)
{
    QCoreApplication::sendEvent(m_icon, e);
    QCoreApplication::sendEvent(m_arrow, e);
}

void Button::mousePressEvent(QMouseEvent *e)
{
    QCoreApplication::sendEvent(m_icon, e);
    QCoreApplication::sendEvent(m_arrow, e);
}

void Button::mouseReleaseEvent(QMouseEvent *e)
{
    QCoreApplication::sendEvent(m_icon, e);
    QCoreApplication::sendEvent(m_arrow, e);

    emitClicked();
}

void Button::onMousePressed(QMouseEvent* e)
{
    mousePressEvent(e);
}

void Button::onMouseReleased(QMouseEvent* e)
{
    mouseReleaseEvent(e);
}
