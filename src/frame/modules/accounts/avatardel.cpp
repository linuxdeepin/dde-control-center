/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
