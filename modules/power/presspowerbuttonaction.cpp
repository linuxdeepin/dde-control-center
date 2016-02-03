/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "presspowerbuttonaction.h"

PressPowerButtonAction::PressPowerButtonAction(QStringList powerReaction, QWidget* parent)
    : QFrame(parent)
{
    setFixedHeight(60);
    m_pressPowerReActionLine=new DHeaderLine;
    m_pressPowerReActionExpand=new DBaseExpand;
    m_pressPowerReActionExpand->setExpandedSeparatorVisible(false);
    m_pressPowerReActionExpand->setHeader(m_pressPowerReActionLine);
    m_powerActionButtonGrid=new DButtonGrid(1, 3);
    m_powerActionButtonGrid->setItemSize(100, 30);

    m_powerActionButtonGrid->addButtons(powerReaction);
    m_powerActionButtonGrid->checkButtonByIndex(2);
    m_pressPowerReActionExpand->setContent(m_powerActionButtonGrid);
    m_pressPowerReActionExpand->setExpand(true);

    m_pressPowerSeparatorLine = new DSeparatorHorizontal;

    pressPowerButtonActionLayout=new QVBoxLayout;
    pressPowerButtonActionLayout->setMargin(0);
    pressPowerButtonActionLayout->setSpacing(0);
    pressPowerButtonActionLayout->addWidget(m_pressPowerReActionLine);
    pressPowerButtonActionLayout->addWidget(m_pressPowerReActionExpand);
    pressPowerButtonActionLayout->addWidget(m_pressPowerSeparatorLine);
    setLayout(pressPowerButtonActionLayout);

    initConnection();
}
void PressPowerButtonAction::setTitle(QString titleName) {
    m_pressPowerReActionLine->setTitle(titleName);
}
void PressPowerButtonAction::initConnection() {
    connect(m_powerActionButtonGrid, SIGNAL(buttonCheckedIndexChanged(int)),
            this, SIGNAL(powerButtonIndexChanged(int)));
}

void PressPowerButtonAction::setPowerButtonAction(int buttonId) {
    switch (buttonId) {
        case 1: { m_powerActionButtonGrid->checkButtonByIndex(1);break;}
        case 2: { m_powerActionButtonGrid->checkButtonByIndex(0);break;}
        default: { m_powerActionButtonGrid->checkButtonByIndex(2);break;}
    }
}
PressPowerButtonAction::~PressPowerButtonAction() {

}
