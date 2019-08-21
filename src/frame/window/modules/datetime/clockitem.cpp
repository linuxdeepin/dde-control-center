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

ClockItem::ClockItem(QWidget *parent, bool isDisplay) :
    SettingsItem(parent),
    m_clock(new Clock),
    m_label(new NormalLabel),
    m_labelTime(new NormalLabel),
    m_labelDate(new NormalLabel),
    m_bIs24HourType(0)
{
    m_clock->setFixedSize(224, 224);
    m_clock->setAutoNightMode(false);
    updateDateTime();

    QLabel *test = new QLabel;
    test->setStyleSheet("background: #f8f8f8");

    QVBoxLayout *layout = new QVBoxLayout;

    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addSpacing(20 * 2);
    layout->addWidget(m_clock, 0, Qt::AlignHCenter);
    layout->addSpacing(20);

    if (isDisplay) {
        layout->addWidget(m_labelTime, 1, Qt::AlignHCenter);
        layout->addSpacing(20);

        QHBoxLayout *timeLayout = new QHBoxLayout;
        timeLayout->addWidget(m_label, 0, Qt::AlignLeft);
        timeLayout->addWidget(m_labelDate, 1, Qt::AlignRight);
        layout->addLayout(timeLayout);
    }

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

void ClockItem::setTimeHourType(bool type)
{
    //type : true -> 24 hour  ,  false -> 12 hour
    if (m_bIs24HourType != type) {
        m_bIs24HourType = type;
        updateDateTime();
    }
}

void ClockItem::translateHourType()
{
    QTime currentTime = QTime::currentTime();
    int nHour = currentTime.hour();

    //[0,23]
    if (currentTime.hour() > 12) {
        nHour -= 12;
    }

    m_labelTime->setText(QString("%1:%2:%3")
                         .arg(nHour, 2, 10, QLatin1Char('0'))
                         .arg(currentTime.minute(), 2, 10, QLatin1Char('0'))
                         .arg(currentTime.second(), 2, 10, QLatin1Char('0')));
}

void ClockItem::updateDateTime()
{
    m_clock->setTimeZone(m_zoneInfo);
    m_clock->update();

    if (m_bIs24HourType) {
        m_labelTime->setText(QDateTime::currentDateTime().toString("HH:mm:ss"));
    } else {
        translateHourType();
    }
    m_label->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    m_labelDate->setText(QDate::currentDate().toString("dddd"));
}
