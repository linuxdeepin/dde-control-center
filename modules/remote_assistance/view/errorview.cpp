/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "errorview.h"

#include <QDebug>
#include <QLabel>

#include <dthememanager.h>

#include "constants.h"
#include "../helper.h"

DWIDGET_USE_NAMESPACE

ErrorView::ErrorView(QWidget* p)
    : AbstractView(p),
      m_text(new QLabel)
{
    setObjectName("ErrorView");
    initialize();
}

QWidget* ErrorView::createMainWidget()
{
    m_text->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_text->setFixedSize(DCC::ModuleContentWidth, 70);
    m_text->setAlignment(Qt::AlignCenter);
    m_text->setWordWrap(true);
    setStyleSheet(readStyleSheet("errorview"));
    return m_text;
}

ErrorView* ErrorView::setText(const QString& text)
{
    m_text->setText(text);
    return this;
}
