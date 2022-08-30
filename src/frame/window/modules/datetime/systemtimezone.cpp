// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
    //~ child_page Timezone List
    m_setSystemTimezone->setText(tr("Change System Timezone"));
    m_systemTimezone->addBackground();

    //与背景间距20,外框设置了10,这里只需设置10就够了
    const int titleLeftMargin = 10;

    //~ contents_path /datetime/Timezone List
    //~ child_page Timezone List
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
