/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "abstractview.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <dseparatorhorizontal.h>
#include <dtextbutton.h>

#include "constants.h"

DUI_USE_NAMESPACE

AbstractView::AbstractView(QWidget* p)
    : QWidget(p),
      m_mainWidget(nullptr),
      m_viewLayout(new QVBoxLayout(this)),
      m_buttonLayout(new QHBoxLayout)
{
}

void AbstractView::initialize()
{
    m_viewLayout->setSpacing(0);
    m_viewLayout->setMargin(0);

    m_mainWidget = createMainWidget();

    m_viewLayout->addWidget(m_mainWidget);
    m_viewLayout->addWidget(new DSeparatorHorizontal);
    m_viewLayout->addSpacing(5);
    m_viewLayout->addLayout(m_buttonLayout);

    m_buttonLayout->addStretch();
    m_buttonLayout->addSpacing(8);
}

AbstractView* AbstractView::addWidget(QWidget* w)
{
    m_viewLayout->insertWidget(m_viewLayout->count()-2, w);
    return this;
}

AbstractView* AbstractView::addSpacing(int spacing)
{
    m_viewLayout->insertSpacing(m_viewLayout->count()-2, spacing);
    return this;
}


AbstractView* AbstractView::addLayout(QLayout* l, int stretch)
{
    m_viewLayout->insertLayout(m_viewLayout->count()-2, l, stretch);
    return this;
}

AbstractView* AbstractView::addButton(DTextButton* btn, int stretch, Qt::Alignment alignment)
{
    m_buttonLayout->insertSpacing(m_buttonLayout->count()-1, 5);
    m_buttonLayout->insertWidget(m_buttonLayout->count()-1, btn, stretch, alignment);
    return this;
}
