/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#include "abstractsection.h"
#include "widgets/settingshead.h"

#include <QVBoxLayout>

using namespace DCC_NAMESPACE::network;
using namespace dcc::widgets;

AbstractSection::AbstractSection(QFrame *parent)
    : QFrame(parent)
{
    m_group = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);
    m_layout = new QVBoxLayout;
    m_layout->addWidget(m_group);
    setLayout(m_layout);
}

AbstractSection::AbstractSection(const QString &title, QFrame *parent)
    : AbstractSection(parent)
{
    SettingsHead *head = new SettingsHead();
    head->setTitle(title);
    head->setEditEnable(false);

    // 需要与列表中文字对齐，列表文字与边框边距为8
    head ->setContentsMargins(8, 0, 0, 0);
    m_layout->insertWidget(0, head);
}

AbstractSection::~AbstractSection()
{
}

void AbstractSection::appendItem(SettingsItem *item)
{
    m_group->appendItem(item);
}

void AbstractSection::insertItem(int idx, SettingsItem *item)
{
    m_group->insertItem(idx, item);
}

void AbstractSection::setSettingsHead(SettingsHead *settingsHead)
{
    m_layout->insertWidget(0, settingsHead);
}
