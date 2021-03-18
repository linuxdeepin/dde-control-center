/*
 * Copyright (C) 2020 Deepin Technology Co., Ltd.
 *
 * Author:     xiaoyaobing <xiaoyaobing@uniontech.com>
 *
 * Maintainer: xiaoyaobing <xiaoyaobing@uniontech.com>
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

#include "batteryhealthwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <DLabel>
#include <DSwitchButton>
#include <DFontSizeManager>

DWIDGET_USE_NAMESPACE

BatteryHealthwidget::BatteryHealthwidget(QWidget *parent)
    : QWidget(parent)
    , m_battryOptSwitch(new DSwitchButton)
{
    initUI();
}

void BatteryHealthwidget::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(10, 20, 10, 0);

    setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout *switchLayout = new QHBoxLayout;
    DLabel *switcheLabel = new DLabel(tr("Optimize battery charging"));
    DFontSizeManager::instance()->bind(switcheLabel, DFontSizeManager::T5, QFont::DemiBold);
    switchLayout->addWidget(switcheLabel);
    switchLayout->addStretch();
    switchLayout->addWidget(m_battryOptSwitch);
    mainLayout->addLayout(switchLayout);

    DLabel *labelTip = new DLabel(tr("In order to slow down battery aging, optimize battery charging"));
    DFontSizeManager::instance()->bind(labelTip, DFontSizeManager::T8);
    labelTip->adjustSize();
    labelTip->setWordWrap(true);
    labelTip->setContentsMargins(10, 5, 10, 5);
    mainLayout->addWidget(labelTip);

    setLayout(mainLayout);
}
