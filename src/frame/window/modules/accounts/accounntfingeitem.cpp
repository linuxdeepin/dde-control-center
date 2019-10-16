/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#include "accounntfingeitem.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

using namespace dcc;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

AccounntFingeItem::AccounntFingeItem(QWidget *parent)
    : SettingsItem(parent)
    , m_layout(new QHBoxLayout)
    , m_title(new QLabel)
{
    setFixedHeight(36);

    m_layout->setContentsMargins(20, 0, 10, 0);
    m_layout->addWidget(m_title, 0, Qt::AlignLeft);

    setLayout(m_layout);
}


void AccounntFingeItem::setTitle(const QString &title)
{
    if (title.isEmpty()) {
        m_layout->removeWidget(m_title);
    } else {
        m_title->setText(title);
    }
}

void AccounntFingeItem::appendItem(QWidget *widget)
{
    m_layout->addWidget(widget, 0, Qt::AlignLeft);
}
