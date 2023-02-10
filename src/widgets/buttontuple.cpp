//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/buttontuple.h"
#include "widgets/accessibleinterface.h"

#include <QHBoxLayout>
#include <QPushButton>

#include <DSuggestButton>
#include <DWarningButton>
#include <DPushButton>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;
SET_FORM_ACCESSIBLE(ButtonTuple,"ButtonTuple")
ButtonTuple::ButtonTuple(ButtonType type, QWidget *parent)
    : QWidget(parent)
    , m_leftButton(new QPushButton(this))
    , m_rightButton(nullptr)
{
    setAccessibleName("ButtonTuple");
    initUI();
    setButtonType(type);
}

void ButtonTuple::setButtonType(const ButtonType type)
{
    if (m_rightButton) {
        layout()->removeWidget(m_rightButton);
        m_rightButton->setVisible(false);
        m_rightButton->setParent(nullptr);
        m_rightButton->deleteLater();
    }
    switch (type) {
    case Save:
        m_rightButton = new DSuggestButton(this);
        break;
    case Delete:
        m_rightButton = new DWarningButton(this);
        break;
    case Normal:
        Q_FALLTHROUGH();
    default:
        m_rightButton = new DPushButton(this);
        break;
    }
    layout()->addWidget(m_rightButton);
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

void ButtonTuple::initUI()
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_leftButton);
    layout->addSpacing(10);
    // m_rightButton在setButtonType中初始化
    setLayout(layout);

    connect(m_leftButton, &QPushButton::clicked, this, &ButtonTuple::leftButtonClicked);
}
