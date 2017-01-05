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

AvatarDel::AvatarDel(QWidget *parent) : QWidget(parent)
{
    setFixedSize(PIX_SIZE, PIX_SIZE);
    DImageButton *close = new DImageButton;
    close->setText("X");
    close->setStyleSheet("QLabel {color : black; }");
    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->setMargin(0);
    mainlayout->setSpacing(0);
    mainlayout->addWidget(close, 0, Qt::AlignHCenter);
    mainlayout->setAlignment(Qt::AlignBottom);
    setLayout(mainlayout);
    connect(close, &DImageButton::clicked, this, &AvatarDel::click);
}

void AvatarDel::paintEvent(QPaintEvent *e)
{
    QPainterPath painterPath;
    painterPath.addEllipse(rect());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setClipPath(painterPath);

    painter.fillRect(0, height() / 1.5, width(), height(), Qt::white);

    QWidget::paintEvent(e);
}
