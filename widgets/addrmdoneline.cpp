/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QDebug>

#include "dynamiclabel.h"
#include "addrmdoneline.h"
#include "imagenamebutton.h"

AddRmDoneLine::AddRmDoneLine(QWidget *parent) :
    DBaseLine(parent),
    m_leftLabel(new DBreathingLabel(this)),
    m_dynamicLabel(new DynamicLabel)
{
    QFrame *frame = new QFrame(this);
    QHBoxLayout *layout = new QHBoxLayout;
    m_removeButton = new ImageNameButton("delete");
    m_addButton = new ImageNameButton("add");
    m_doneButton = new DTextButton(tr("Done"));
    m_doneButton->hide();

    m_leftLabel->move(15, 0);
    m_leftLabel->setStyleSheet("font-size: 12px;");
    m_leftLabel->setColor("#b4b4b4");

    layout->setMargin(0);
    layout->addWidget(m_dynamicLabel);
    layout->addWidget(m_removeButton);
    layout->addWidget(m_addButton);
    layout->addWidget(m_doneButton);

    connect(m_removeButton, &ImageNameButton::clicked, this, &AddRmDoneLine::removeClicked);
    connect(m_addButton, &ImageNameButton::clicked, this, &AddRmDoneLine::addClicked);
    connect(m_doneButton, &DTextButton::clicked, this, &AddRmDoneLine::doneClicked);
    connect(this, &AddRmDoneLine::setAddHidden, m_addButton, &ImageNameButton::setHidden);
    connect(this, &AddRmDoneLine::setRemoveHidden, m_removeButton, &ImageNameButton::setHidden);
    connect(this, &AddRmDoneLine::setDoneHidden, m_doneButton, &DTextButton::setHidden);
    connect(m_dynamicLabel, &DynamicLabel::hideFinished, m_leftLabel, &QLabel::show);
    connect(m_removeButton, &DImageButton::stateChanged, this, &AddRmDoneLine::onButtonStateChanged);
    connect(m_addButton, &DImageButton::stateChanged, this, &AddRmDoneLine::onButtonStateChanged);

    frame->setLayout(layout);

    setRightContent(frame);
    setFixedHeight(EXPAND_HEADER_HEIGHT);
}

void AddRmDoneLine::setTitle(const QString &title)
{
    m_leftLabel->setText(title);
    m_leftLabel->setFixedWidth(m_leftLabel->sizeHint().width());
}

ImageNameButton *AddRmDoneLine::removeButton() const
{
    return m_removeButton;
}

ImageNameButton *AddRmDoneLine::addButton() const
{
    return m_addButton;
}

DTextButton *AddRmDoneLine::doneButton() const
{
    return m_doneButton;
}

QString AddRmDoneLine::rmButtonToolTip() const
{
    return m_rmButtonToolTip;
}

QString AddRmDoneLine::addButtonToolTip() const
{
    return m_addButtonToolTip;
}

QString AddRmDoneLine::doneButtonToolTip() const
{
    return m_doneButtonToolTip;
}

void AddRmDoneLine::setRmButtonToolTip(QString rmButtonToolTip)
{
    m_rmButtonToolTip = rmButtonToolTip;
}

void AddRmDoneLine::setAddButtonToolTip(QString addButtonToolTip)
{
    m_addButtonToolTip = addButtonToolTip;
}

void AddRmDoneLine::setDoneButtonToolTip(QString doneButtonToolTip)
{
    m_doneButtonToolTip = doneButtonToolTip;
}

void AddRmDoneLine::onButtonStateChanged()
{
    DImageButton *button = qobject_cast<DImageButton*>(sender());
    if(!button)
        return;

    if(button->getState() == DImageButton::Hover){
        QString tooltip = button == m_addButton ? m_addButtonToolTip : m_rmButtonToolTip;
        if(tooltip.isEmpty())
            return;

        m_dynamicLabel->setText(tooltip);
        m_dynamicLabel->showLabel();
        ///230 is m_dynamicLabel geometry right border position.
        if(m_leftLabel->geometry().right() >= 230 - m_dynamicLabel->fontMetrics().width(tooltip))
            m_leftLabel->hideLabel();
    }else{
        m_dynamicLabel->hideLabel();
        if(!m_leftLabel->property("alpha").toBool())
            m_leftLabel->showLabel();
    }
}

