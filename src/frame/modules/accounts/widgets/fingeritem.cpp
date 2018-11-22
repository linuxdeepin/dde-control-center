/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
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

#include "fingeritem.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::accounts;

FingerItem::FingerItem(QWidget *parent)
    : SettingsItem(parent)
    , m_layout(new QHBoxLayout)
    , m_editBtn(new DImageButton)
    , m_removeBtn(new DImageButton)
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
