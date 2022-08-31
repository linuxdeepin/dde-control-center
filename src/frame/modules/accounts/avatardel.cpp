// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
    close->setNormalPic(":/accounts/themes/common/icons/avator_delete_normal.svg");
    close->setHoverPic(":/accounts/themes/common/icons/avator_delete_hover.svg");
    close->setPressPic(":/accounts/themes/common/icons/avator_delete_press.svg");

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

    painterPath.moveTo(width(), height() / 1.5);
    painterPath.lineTo(0, height() / 1.5);
    painterPath.arcTo(rect(), -160, 140);

    setMaskPath(painterPath);
}
