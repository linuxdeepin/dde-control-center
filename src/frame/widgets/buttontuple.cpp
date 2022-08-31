// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "widgets/buttontuple.h"

#include <QHBoxLayout>
#include <QPushButton>

#include <QDebug>

namespace dcc {
namespace widgets {

ButtonTuple::ButtonTuple(ButtonType type, QWidget *parent)
    : QWidget(parent)
    , m_leftButton(new LeftButton)
{
    setAccessibleName("ButtonTuple");
    createRightButton(type);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_leftButton);
    layout->addSpacing(10);
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

void ButtonTuple::removeSpacing()
{
    if (!this->layout())
        return;
    //第二个控件为space
    if (this->layout()->itemAt(1)) {
        this->layout()->removeItem(this->layout()->itemAt(1));
    }
}

void ButtonTuple::createRightButton(const ButtonTuple::ButtonType type)
{
    switch (type) {
    case Save:
        m_rightButton = new DSuggestButton();
        break;
    case Delete:
        m_rightButton = new DWarningButton();
        break;
    case Normal:
        Q_FALLTHROUGH();
    default:
        m_rightButton = new DPushButton();
        break;
    }
}
} // namespace widgets
} // namespace dcc
