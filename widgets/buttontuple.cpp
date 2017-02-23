/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "buttontuple.h"

#include <QHBoxLayout>
#include <QPushButton>

#include <QDebug>

namespace dcc {
namespace widgets {

ButtonTuple::ButtonTuple(QWidget *parent) :
    QWidget(parent),
    m_leftButton(new LeftButton),
    m_rightButton(new RightButton)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_leftButton);
    layout->addSpacing(1);
    layout->addWidget(m_rightButton);
    setLayout(layout);

    connect(m_leftButton, &QPushButton::clicked, this, &ButtonTuple::leftButtonClicked);
    connect(m_rightButton, &QPushButton::clicked, this, &ButtonTuple::rightButtonClicked);
}

QPushButton *ButtonTuple::leftButton()
{
    return m_leftButton;
}

QPushButton *ButtonTuple::rightButton()
{
    return m_rightButton;
}

} // namespace widgets
} // namespace dcc
