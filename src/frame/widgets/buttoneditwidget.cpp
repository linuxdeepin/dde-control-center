// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "widgets/buttoneditwidget.h"
#include "widgets/lineeditwidget.h"
#include "window/utils.h"

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
    m_title->setMinimumWidth(DCC_NAMESPACE::LeftTitleWitdh);
    m_edit->setContextMenuPolicy(Qt::NoContextMenu);

    m_addBtn->setFixedSize(QSize(19, 19));
    m_addBtn->setIconSize(QSize(17, 17));

    m_reduceBtn->setFixedSize(QSize(19, 19));
    m_reduceBtn->setIconSize(QSize(17, 17));

    QHBoxLayout *rightLayout = new QHBoxLayout;
    rightLayout->addWidget(m_edit, 0, Qt::AlignVCenter);
    rightLayout->addWidget(m_addBtn, 0, Qt::AlignVCenter);
    rightLayout->addWidget(m_reduceBtn, 0, Qt::AlignVCenter);

    m_mainLayout = new QHBoxLayout;
    m_mainLayout->addWidget(m_title, 3, Qt::AlignVCenter);
    m_mainLayout->addLayout(rightLayout, 7);

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
