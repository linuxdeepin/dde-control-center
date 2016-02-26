/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "generatingview.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

#include <libdui/dthememanager.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dtextbutton.h>
#include <libdui/dloadingindicator.h>

#include "constants.h"
#include "../helper.h"

DUI_USE_NAMESPACE

GeneratingView::GeneratingView(QWidget* p)
    : AbstractView(p)
{
    setObjectName("GeneratingView");
    initialize();

    auto button = new DTextButton(tr("Cancel"));
    connect(button, SIGNAL(clicked(bool)), this, SLOT(onCancelButtonClicked()));
    addButton(button);

    setStyleSheet(readStyleSheet("generatingview"));
}

void GeneratingView::onCancelButtonClicked()
{
    emit cancel();
}

QWidget* GeneratingView::createMainWidget()
{
    QWidget* mainWidget = new QWidget;
    auto mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->addStretch();

    auto loadingIndicator = new DLoadingIndicator(mainWidget);
    loadingIndicator->setFixedSize(16,16);
    loadingIndicator->setImageSource(QPixmap(getThemeImage("waiting.png")));
    loadingIndicator->setAniDuration(720);
    loadingIndicator->setLoading(true);

    mainLayout->addWidget(loadingIndicator);

    mainLayout->addSpacing(21);

    QLabel* text = new QLabel;
    text->setWordWrap(true);
    text->setAlignment(Qt::AlignVCenter);
    text->setText(tr("Generating the verification code, please wait..."));

    mainLayout->addWidget(text);
    mainLayout->addStretch();

    mainWidget->setFixedHeight(70);
    mainWidget->setLayout(mainLayout);

    return mainWidget;
}
