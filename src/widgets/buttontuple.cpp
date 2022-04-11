/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "widgets/buttontuple.h"

#include <QHBoxLayout>
#include <QPushButton>

#include <DSuggestButton>
#include <DWarningButton>
#include <DPushButton>

DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

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
