/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "soundeffectswitchwidget.h"

#include <QHBoxLayout>

SoundEffectSwitchWidget::SoundEffectSwitchWidget(QWidget *parent)
    : QWidget(parent)
{
    m_soundName = new QLabel;
    m_soundName->setStyleSheet("color:#999;");
    m_switchButton = new DSwitchButton;

    QHBoxLayout *lineLayout = new QHBoxLayout;
    lineLayout->addWidget(m_soundName);
    lineLayout->setAlignment(m_soundName, Qt::AlignVCenter);
    lineLayout->addStretch();
    lineLayout->addWidget(m_switchButton);
    lineLayout->setAlignment(m_switchButton, Qt::AlignVCenter);
    lineLayout->setSpacing(0);
    lineLayout->setContentsMargins(15, 0, 15, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(lineLayout);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    setLayout(mainLayout);
    setFixedHeight(50);

    connect(m_switchButton, &DSwitchButton::checkedChanged, this, &SoundEffectSwitchWidget::switchToggled);
}

