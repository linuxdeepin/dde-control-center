/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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
#include "netswitchwidget.h"

#include "widgets/switchwidget.h"

using namespace dcc;
using namespace widgets;

namespace DCC_NAMESPACE {
namespace network {

NetSwitchWidget::NetSwitchWidget(QFrame *parent)
    : SettingsItem(parent)
{
    m_autoConnItem = new SwitchWidget();
    m_swswitchLayout = new QHBoxLayout();
    m_swithTitle = new QLabel();

    setFixedHeight(36);
    m_autoConnItem->setFixedHeight(24);

    initNetSwitch();
}

void NetSwitchWidget::initNetSwitch()
{
    addBackground();
    setLayout(m_swswitchLayout);

    m_swswitchLayout->setContentsMargins(10, 0, 10, 0);
    m_swswitchLayout->addWidget(m_swithTitle, 0, Qt::AlignLeft);
    m_swswitchLayout->addStretch();
    m_swswitchLayout->addWidget(m_autoConnItem, 0, Qt::AlignLeft);
}

void NetSwitchWidget::setTitle(const QString title)
{
    m_swithTitle->setText(title);
    m_swithTitle->setFixedHeight(36);
}

SwitchWidget *NetSwitchWidget::switchWidget()
{
    return m_autoConnItem;
}

}
}
