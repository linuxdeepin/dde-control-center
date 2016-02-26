/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "connectedview.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>

#include <libdui/dthememanager.h>
#include <dseparatorhorizontal.h>
#include <dtextbutton.h>

#include "constants.h"

#include "../helper.h"

DUI_USE_NAMESPACE

ConnectedView::ConnectedView(QWidget* p)
    : AbstractView(p), m_text(new QLabel)
{
    setObjectName("ConnectedView");
    initialize();

    auto button = new DTextButton(tr("Disconnect"));
    connect(button, SIGNAL(clicked(bool)), this, SLOT(onDisconnectButtonClicked()));
    addButton(button);

    setStyleSheet(readStyleSheet("connectedview"));
}

QWidget* ConnectedView::createMainWidget()
{
    m_text->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_text->setFixedSize(DCC::ModuleContentWidth, 70);
    m_text->setAlignment(Qt::AlignCenter);
    m_text->setWordWrap(true);
    return m_text;
}

ConnectedView* ConnectedView::setText(const QString& text)
{
    m_text->setText(text);
    return this;
}

void ConnectedView::onDisconnectButtonClicked()
{
    emit disconnect();
}
