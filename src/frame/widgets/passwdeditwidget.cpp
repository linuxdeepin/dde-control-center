// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "passwdeditwidget.h"
#include <QDebug>

using namespace dcc;
using namespace dcc::widgets;

PasswdEditWidget::PasswdEditWidget(QFrame *parent)
    : LineEditWidget(parent)
    , m_switchBtn(new DImageButton)
{
    updateBtnPic();

    m_mainLayout->addWidget(m_switchBtn);
    m_mainLayout->addSpacing(10);

    textEdit()->setEchoMode(QLineEdit::Password);

    connect(m_switchBtn, &DImageButton::clicked, this, &PasswdEditWidget::switchMode);
}

void PasswdEditWidget::setSwitchBtnVisible(bool visible)
{
    m_switchBtn->setVisible(visible);
}

void PasswdEditWidget::switchMode()
{
    const bool isPasswdMode = textEdit()->echoMode() == QLineEdit::Password;

    updateBtnPic();

    textEdit()->setEchoMode(isPasswdMode ? QLineEdit::Normal : QLineEdit::Password);
}

void PasswdEditWidget::updateBtnPic()
{
    const bool isPasswdMode = textEdit()->echoMode() == QLineEdit::Password;

    m_switchBtn->setNormalPic(QString(":/widgets/themes/dark/icons/password_%1_normal.svg").arg(isPasswdMode ? "hide" : "show"));
    m_switchBtn->setHoverPic(QString(":/widgets/themes/dark/icons/password_%1_hover.svg").arg(isPasswdMode ? "hide" : "show"));
    m_switchBtn->setPressPic(QString(":/widgets/themes/dark/icons/password_%1_press.svg").arg(isPasswdMode ? "hide" : "show"));
}
