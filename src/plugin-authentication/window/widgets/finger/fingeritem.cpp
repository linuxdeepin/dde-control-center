//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "fingeritem.h"

#include <QBoxLayout>
#include <QLabel>

FingerItem::FingerItem(QWidget *parent)
    : SettingsItem(parent)
    , m_layout(new QHBoxLayout)
    , m_editMode(false)
    , m_editBtn(new DIconButton(this))
    , m_removeBtn(new DIconButton(this))
    , m_title(new QLabel)
{
    setFixedHeight(36);

    m_editBtn->setVisible(false);

    m_layout->setContentsMargins(20, 0, 10, 0);

    m_layout->addWidget(m_title);
    m_layout->addWidget(m_editBtn);
    m_layout->addStretch();
    m_layout->addWidget(m_removeBtn);

    setLayout(m_layout);
}

void FingerItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void FingerItem::setEditMode(const bool editmode)
{
    m_editMode = editmode;

    m_editBtn->setVisible(editmode);
    m_removeBtn->setVisible(editmode);
}
