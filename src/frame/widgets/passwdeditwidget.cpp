/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: kirigaya <kirigaya@mkacg.com>
 *             listenerri <listenerri@gmail.com>
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
