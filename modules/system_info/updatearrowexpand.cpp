/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "updatearrowexpand.h"

#include <libdui/dthememanager.h>

DUI_USE_NAMESPACE

UpdateArrowExpand::UpdateArrowExpand(QWidget *parent) :
    DArrowLineExpand(parent)
{
    D_THEME_INIT_WIDGET(UpdateArrowExpand);

    m_updateCountLabel = new QLabel(this);
    m_updateCountLabel->setFixedHeight(18);
    m_updateCountLabel->setMinimumWidth(18);
    m_updateCountLabel->setAlignment(Qt::AlignCenter);
    m_updateCountLabel->setObjectName("UpdateCount");

    m_configBtn = new DImageButton(this);
    m_configBtn->setNormalPic(":/images/images/config_normal.png");
    m_configBtn->setHoverPic(":/images/images/config_hover.png");
    m_configBtn->setPressPic(":/images/images/config_press.png");

    setTitle(tr("Update"));

    QBoxLayout *headerLeftLayout = headerLine()->leftLayout();
    headerLeftLayout->addWidget(m_updateCountLabel);
    headerLeftLayout->setSpacing(10);
    QBoxLayout *headerRightLayout = headerLine()->rightLayout();
    headerRightLayout->insertWidget(0, m_configBtn);
    headerRightLayout->setSpacing(10);

    connect(m_configBtn, &DImageButton::clicked, this, &UpdateArrowExpand::configButtonClicked);
    connect(m_configBtn, &DImageButton::clicked, [this] {setExpand(true);});
}

void UpdateArrowExpand::setUpdatableNums(int nums)
{
    if (!nums)
        return m_updateCountLabel->hide();

    m_updateCountLabel->show();
    m_updateCountLabel->setText(QString::number(nums));
}
