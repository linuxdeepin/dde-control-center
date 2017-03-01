/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "titlebuttonItem.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QSpacerItem>

TitleButtonItem::TitleButtonItem(QFrame *parent)
    :SettingsItem(parent)
{
    QHBoxLayout* layout =new QHBoxLayout();
    m_title = new QLabel();
    m_button = new QPushButton();

    layout->addWidget(m_title);
    layout->addStretch();
    layout->addWidget(m_button);

    setLayout(layout);

    connect(m_button, SIGNAL(clicked()), this, SIGNAL(click()));
}

void TitleButtonItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void TitleButtonItem::setValue(const QString &value)
{
    m_button->setText(value);
}
