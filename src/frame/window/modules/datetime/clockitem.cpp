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
#include "clock.h"
#include "clockitem.h"
#include "widgets/labels/normallabel.h"

#include <QVBoxLayout>
#include <QTimer>

using namespace dcc::widgets;

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::datetime;

ClockItem::ClockItem(QWidget *parent) :
    SettingsItem(parent),
    m_clock(new Clock),
    m_label(new NormalLabel),
    m_labelTime(new NormalLabel),
    m_labelDate(new NormalLabel)
{
    m_clock->setFixedSize(160, 160);
    m_clock->setAutoNightMode(false);
    updateDateTime();

//    *m_lcdTime = new QLCDNumber(this);
//    // 设置能显示的位数
//    m_lcdTime->setDigitCount(55);
//    // 设置显示的模式为十进制
//    m_lcdTime->setMode(QLCDNumber::Dec);
//    // 设置显示外观
//    m_lcdTime->setSegmentStyle(QLCDNumber::Filled);
//    // 设置样式
//    m_lcdTime->setStyleSheet("border: 1px solid green; color: #657d8e; background: silver;");
//    m_lcdTime->display(QDateTime::currentDateTime().toString("HH:mm:ss").toLatin1().data());

    QLabel *test = new QLabel;
    test->setStyleSheet("background: #f8f8f8");

    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *timeLayout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addSpacing(20 * 2);
    layout->addWidget(m_clock, 0, Qt::AlignHCenter);
    layout->addSpacing(20);
    layout->addWidget(m_labelTime, 1, Qt::AlignHCenter);
    layout->addSpacing(20);
    timeLayout->addWidget(m_label, 0, Qt::AlignLeft);
    timeLayout->addWidget(m_labelDate, 1, Qt::AlignRight);
    layout->addLayout(timeLayout);
    layout->addSpacing(20);
    setLayout(layout);

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, &QTimer::timeout, this, &ClockItem::updateDateTime, Qt::DirectConnection);

    timer->start(1000);
}

void ClockItem::setTimeZone(const ZoneInfo &zone)
{
    if (m_zoneInfo == zone) return;

    m_zoneInfo = zone;

    updateDateTime();
}

void ClockItem::updateDateTime()
{
    m_clock->setTimeZone(m_zoneInfo);
    m_clock->update();
    m_labelTime->setText(QDateTime::currentDateTime().toString("HH:mm:ss"));
    m_label->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    m_labelDate->setText(QDate::currentDate().toString("dddd"));
//    m_lcdTime->display(QDateTime::currentDateTime().toString("hh:mm:ss"));
}
