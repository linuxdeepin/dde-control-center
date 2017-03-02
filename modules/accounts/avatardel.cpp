/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "avatardel.h"
#include "avatarwidget.h"
#include <QPainter>
#include <dimagebutton.h>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

AvatarDel::AvatarDel(QWidget *parent) : DBlurEffectWidget(parent)
{
    setFixedSize(PIX_SIZE, PIX_SIZE);
    DImageButton *close = new DImageButton;
    close->setNormalPic(":/accounts/themes/common/icons/avator_delete_normal.png");
    close->setHoverPic(":/accounts/themes/common/icons/avator_delete_hover.png");
    close->setPressPic(":/accounts/themes/common/icons/avator_delete_press.png");

    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->setMargin(0);
    mainlayout->setSpacing(0);
    mainlayout->addWidget(close, 0, Qt::AlignCenter);
    mainlayout->addSpacing(5);
    mainlayout->setAlignment(Qt::AlignBottom);
    setLayout(mainlayout);
    connect(close, &DImageButton::clicked, this, &AvatarDel::click);

    setMaskColor(Qt::white);

    QPainterPath painterPath;
    painterPath.moveTo(width() /2, height()/1.5);
    painterPath.arcTo(rect(),200, 140);

    setMaskPath(painterPath);
}
