/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#include "systemtimezone.h"

#include "modules/datetime/timezoneitem.h"
#include "window/utils.h"
#include "widgets/titlelabel.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QFont>
#include <DFontSizeManager>

using namespace DCC_NAMESPACE;
using namespace dcc::datetime;
using namespace DCC_NAMESPACE::datetime;

SystemTimezone::SystemTimezone(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
    , m_systemTimezone(new TimezoneItem)
    , m_setSystemTimezone(new QPushButton)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    //~ contents_path /datetime/Timezone List/Change System Timezone
    m_setSystemTimezone->setText(tr("Change System Timezone"));
    m_systemTimezone->addBackground();

    const int titleLeftMargin = 5;
    //~ contents_path /datetime/Timezone List
    TitleLabel *title = new TitleLabel(tr("System Timezone"));
    DFontSizeManager::instance()->bind(title, DFontSizeManager::T5, QFont::DemiBold);
    title->setContentsMargins(titleLeftMargin, 0, 0, 0);
    title->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    m_layout->addWidget(title, 0, Qt::AlignLeft);
    m_layout->addSpacing(3);
    m_layout->addWidget(m_systemTimezone);
    m_layout->addSpacing(3);
    m_layout->addWidget(m_setSystemTimezone);
    m_layout->setMargin(0);
    m_layout->setContentsMargins(ThirdPageContentsMargins);

    setLayout(m_layout);

    m_systemTimezone->setDetailVisible(false);

    connect(m_setSystemTimezone, &QPushButton::clicked, this, &SystemTimezone::requestSetSystemTimezone);
}

void SystemTimezone::setSystemTimezone(const ZoneInfo &zone)
{
    m_systemTimezone->setTimeZone(zone);
}
