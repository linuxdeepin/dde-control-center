/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "datecontrolwidget.h"

#include <QHBoxLayout>

DateControlWidget::DateControlWidget(QWidget *parent) : QWidget(parent)
{
    m_tipsLabel = new DynamicLabel;
    m_tipsLabel->hideLabel();
    m_tipsLabel->setText(tr("Set Date"));

    m_ctrlBtn = new DImageButton;
    m_ctrlBtn->setCheckable(false);
    m_ctrlBtn->setNormalPic(":/icons/setdate_normal.png");
    m_ctrlBtn->setHoverPic(":/icons/setdate_hover.png");
    m_ctrlBtn->setPressPic(":/icons/setdate_press.png");

    m_confirmBtn = new DTextButton(tr("Confirm"));
    m_cancelBtn = new DTextButton(tr("Cancel"));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addStretch();
    mainLayout->addWidget(m_tipsLabel);
    mainLayout->addWidget(m_ctrlBtn);
    mainLayout->addWidget(m_cancelBtn);
    mainLayout->addWidget(m_confirmBtn);
    mainLayout->setMargin(0);

    connect(m_cancelBtn, &DTextButton::clicked, this, &DateControlWidget::toNormalMode);
    connect(m_confirmBtn, &DTextButton::clicked, this, &DateControlWidget::toNormalMode);
    connect(m_confirmBtn, &DTextButton::clicked, this, &DateControlWidget::applyDate);
    connect(m_ctrlBtn, &DImageButton::clicked, this, &DateControlWidget::toConfirmMode);
    connect(m_ctrlBtn, &DImageButton::stateChanged, [this] () -> void {
        if (m_ctrlBtn->getState() == DImageButton::Hover)
            m_tipsLabel->showLabel();
        else
            m_tipsLabel->hideLabel();
    });

    setLayout(mainLayout);

    toNormalMode();
}

void DateControlWidget::toNormalMode()
{
    m_ctrlBtn->show();
    m_confirmBtn->hide();
    m_cancelBtn->hide();
}

void DateControlWidget::toConfirmMode()
{
    m_ctrlBtn->hide();
    m_confirmBtn->show();
    m_cancelBtn->show();
}
