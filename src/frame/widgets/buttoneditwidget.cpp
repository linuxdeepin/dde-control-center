/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#include "widgets/buttoneditwidget.h"
#include "widgets/lineeditwidget.h"

#include <dpasswordedit.h>
#include <QVBoxLayout>
#include <DLineEdit>
#include <DIconButton>

namespace dcc {
namespace widgets {

ButtonEditWidget::ButtonEditWidget(QFrame *parent)
    : SettingsItem (parent)
    , m_title(new QLabel)
    , m_errTip(new ErrorTip(this))
    , m_edit(new DLineEdit)
    , m_addBtn(new DIconButton(DStyle::SP_AddButton, this))
    , m_reduceBtn(new DIconButton(DStyle::SP_DeleteButton, this))
{
    m_title->setFixedWidth(110);
    m_edit->setContextMenuPolicy(Qt::NoContextMenu);

    m_addBtn->setFixedSize(QSize(19, 19));
    m_addBtn->setIconSize(QSize(17, 17));

    m_reduceBtn->setFixedSize(QSize(19, 19));
    m_reduceBtn->setIconSize(QSize(17, 17));

    m_mainLayout = new QHBoxLayout;

    m_mainLayout->addWidget(m_title, 0, Qt::AlignVCenter);
    m_mainLayout->addWidget(m_edit, 0, Qt::AlignVCenter);
    m_mainLayout->addWidget(m_addBtn, 0, Qt::AlignVCenter);
    m_mainLayout->addWidget(m_reduceBtn, 0, Qt::AlignVCenter);

    setLayout(m_mainLayout);
    setObjectName("ButtonEditWidget");
    initConnect();
}

ButtonEditWidget::~ButtonEditWidget()
{

}

void ButtonEditWidget::initConnect()
{
    connect(m_addBtn, &DIconButton::clicked, this, &ButtonEditWidget::addNewDnsEdit);
    connect(m_reduceBtn, &DIconButton::clicked, this, &ButtonEditWidget::deleteCurrentDnsEdit);
    connect(m_edit, &DLineEdit::textChanged, this, &ButtonEditWidget::hideAlertMessage);
}

void ButtonEditWidget::hideIconBtn()
{
    m_addBtn->hide();
    m_reduceBtn->hide();
}

void ButtonEditWidget::showIconBtn()
{
    m_addBtn->show();
    m_reduceBtn->show();
}

void ButtonEditWidget::hideAlertMessage()
{
    m_edit->setAlert(false);
    setIsErr(false);
    m_errTip->hide();
}

void ButtonEditWidget::setTitle(const QString &title)
{
    m_title->setText(title);
    m_title->setWordWrap(true);

    setAccessibleName(title);
    m_edit->setAccessibleName(title);
}

void ButtonEditWidget::setText(const QString &text)
{
    m_edit->setText(text);
}

} // namespace ButtonEditWidget
} // namespase dcc
