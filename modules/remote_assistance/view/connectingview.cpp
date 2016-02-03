/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "connectingview.h"

#include <QLabel>
#include <QVBoxLayout>

#include <libdui/dthememanager.h>
#include <dloadingindicator.h>
#include <dtextbutton.h>

#include "constants.h"

#include "../helper.h"

DUI_USE_NAMESPACE

ConnectingView::ConnectingView(QWidget*p)
    : AbstractView(p)
{
    setObjectName("ConnectingView");
    initialize();

    auto button = new DTextButton(tr("Cancel"));
    connect(button, SIGNAL(clicked(bool)), this, SLOT(onCancelButtonClicked()));
    addButton(button);
}

QWidget* ConnectingView::createMainWidget()
{
    auto mainWidget = new QWidget;
    mainWidget->setFixedSize(DCC::ModuleContentWidth, 140);
    auto mainLayout = new QHBoxLayout(mainWidget);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    mainLayout->addSpacing(15);

    auto wrap = new QWidget;
    mainLayout->addWidget(wrap);

    mainLayout->addSpacing(15);

    auto wrapLayout = new QVBoxLayout(wrap);

    auto displayWidget = new QWidget;
    auto displayLayout = new QHBoxLayout(displayWidget);
    displayLayout->addStretch();

    auto loadingImage = new DLoadingIndicator(displayWidget);
    loadingImage->setFixedSize(16, 16);
    
    loadingImage->setImageSource(QPixmap(getThemeImage("waiting.png")));
    loadingImage->setAniDuration(720);
    loadingImage->setLoading(true);
    displayLayout->addWidget(loadingImage);

    auto label = new QLabel;
    label->setObjectName("msg");
    label->setText(tr("Establishing connection, please wait..."));
    label->setAlignment(Qt::AlignVCenter);
    displayLayout->addWidget(label);

    displayLayout->addStretch();
    wrapLayout->addWidget(displayWidget);

    auto line = new QWidget;
    line->setObjectName("separator");
    line->setFixedHeight(1);
    wrapLayout->addWidget(line);
    wrapLayout->addSpacing(10);

    auto waitingText = new QLabel;
    waitingText->setObjectName("waitingText");
    waitingText->setWordWrap(true);
    waitingText->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
    waitingText->setText(tr("This panel will be hidden automatically and the remote session window will be opened on the desktop after connection is established successfully"));

    wrapLayout->addWidget(waitingText);
    setStyleSheet(readStyleSheet("connectingview"));
    return mainWidget;
}

void ConnectingView::onCancelButtonClicked()
{
    emit cancel();
}
