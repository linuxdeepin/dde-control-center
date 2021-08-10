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

#include <DTipLabel>

#include <QVBoxLayout>
#include <QTimer>
#include <QFontDatabase>
#include <QDebug>

DWIDGET_USE_NAMESPACE
using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::datetime;

ClockItem::ClockItem(QWidget *parent, bool isDisplay)
    : QWidget(parent)
    , m_clock(new Clock)
    , m_label(nullptr)
    , m_labelTime(nullptr)
    , m_labelDate(nullptr)
    , m_timeType(nullptr)
    , m_bIs24HourType(false)
    , m_bIsEnglishType(false)
    , m_timedateInter(new Timedate("com.deepin.daemon.Timedate", "/com/deepin/daemon/Timedate", QDBusConnection::sessionBus(), this))
    , m_weekdayFormat("dddd")
    , m_shortDateFormat("yyyy-MM-dd")
    , m_longTimeFormat("HH:mm:ss")
    , m_timeLayout(new QHBoxLayout)
{
    m_clock->setAccessibleName("ClockItem_clock");
    m_clock->setMinimumSize(210, 210);
    m_clock->setAutoNightMode(false);
    updateDateTime();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_clock, 0, Qt::AlignHCenter);

    if (isDisplay) {
        m_labelTime = new NormalLabel;
        m_timeType = new DTipLabel("");

        QVBoxLayout *tlayout = new QVBoxLayout;
        tlayout->setMargin(0);
        QHBoxLayout *topLayout = new QHBoxLayout;
        topLayout->addWidget(m_labelTime, 0, Qt::AlignHCenter);
        topLayout->addWidget(m_timeType, 0, Qt::AlignRight | Qt::AlignBottom);

        int nIndex = QFontDatabase::addApplicationFont(":/datetime/resource/deepindigitaltimes-Regular.ttf");
        if (nIndex != -1) {
            QStringList strList(QFontDatabase::applicationFontFamilies(nIndex));
            if (strList.count() > 0) {
                QFont ft(strList.at(0));
                ft.setPointSize(33);

                QFontMetricsF fm(ft);
                auto rt1 = fm.boundingRect("33:33:33");

                QFontMetricsF fm2(m_timeType->font());
                auto rt2 = fm2.boundingRect(tr("AM"));
                m_timeType->setContentsMargins(0, 0, 0, int(rt1.bottom() - rt2.bottom()));
            }
        }

        auto twidget = new QWidget();
        twidget->setAccessibleName("topLayout");
        twidget->setContentsMargins(0, 0, 0, 0);
        twidget->setLayout(topLayout);
        twidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        tlayout->addWidget(twidget, 0, Qt::AlignVCenter);

        m_label = new DTipLabel("");
        m_label->setAccessibleName("ClockItem_label");
        m_labelDate = new DTipLabel("");
        m_labelDate->setAccessibleName("ClockItem_labelDate");
        m_timeLayout->addWidget(m_label, 0, Qt::AlignLeft);
        m_timeLayout->addWidget(m_labelDate, 0, Qt::AlignRight);
        tlayout->addLayout(m_timeLayout);

        SettingsItem *item = new SettingsItem;

        twidget = new QWidget();
        twidget->setAccessibleName("tlayout");
        twidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        twidget->setLayout(tlayout);

        QVBoxLayout *itemlayout = new QVBoxLayout;
        itemlayout->addWidget(twidget, 0, Qt::AlignHCenter);
        item->addBackground();
        item->setLayout(itemlayout);
        layout->addWidget(item, 0, Qt::AlignVCenter);
    }

    setLayout(layout);

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, &QTimer::timeout, this, &ClockItem::updateDateTime, Qt::DirectConnection);

    timer->start(1000);

    setWeekdayFormatType(m_timedateInter->weekdayFormat());
    setShortDateFormat(m_timedateInter->shortDateFormat());
    setLongTimeFormat(m_timedateInter->longTimeFormat());

    connect(m_timedateInter, &Timedate::WeekdayFormatChanged, this, &ClockItem::setWeekdayFormatType);
    connect(m_timedateInter, &Timedate::ShortDateFormatChanged, this, &ClockItem::setShortDateFormat);
    connect(m_timedateInter, &Timedate::LongTimeFormatChanged, this, &ClockItem::setLongTimeFormat);
}

ClockItem::~ClockItem()
{
    delete  m_timeLayout;
    m_timeLayout = nullptr;
}

void ClockItem::setTimeZone(const ZoneInfo &zone)
{
    if (m_zoneInfo == zone) return;

    m_zoneInfo = zone;

    updateDateTime();
}

void ClockItem::setTimeHourType(bool type)
{
    if (m_bIs24HourType == type)
        return;

    m_bIs24HourType = type;

    updateDateTime();
}

void ClockItem::setTimeEnglishType(bool type)
{
    if (type != m_bIsEnglishType) {
        m_bIsEnglishType = type;
        updateDateTime();
    }
}

void ClockItem::setPlate(bool state)
{
    if (m_clock) {
        m_clock->setPlate(state);
    }
}

void ClockItem::translateHourType()
{
    if (!(m_labelTime && m_timeType))
        return;

    QTime currentTime = QTime::currentTime();
    int nHour = currentTime.hour();

    //[0,23]
    if (currentTime.hour() == 0) {
        nHour = 12;
    } else if (currentTime.hour() > 12) {
        nHour -= 12;
    }

    int nIndex = QFontDatabase::addApplicationFont(":/datetime/resource/deepindigitaltimes-Regular.ttf");
    if (nIndex != -1) {
        QStringList strList(QFontDatabase::applicationFontFamilies(nIndex));
        if (strList.count() > 0) {
            QFont fontThis(strList.at(0));
            fontThis.setPointSize(28);
            m_labelTime->setFont(fontThis);
        }
    }
	//12小时格式显示的时候长日期不会显示个位数
    m_labelTime->setText(QString("%1:%2:%3")
                         .arg(nHour, (m_longTimeFormat == "h:mm:ss" && nHour < 10) ? 1 : 2, 10, QLatin1Char('0'))
                         .arg(currentTime.minute(), 2, 10, QLatin1Char('0'))
                         .arg(currentTime.second(), 2, 10, QLatin1Char('0')));

    m_timeType->setText(currentTime.hour() >= 12 ? tr("PM") : tr("AM"));
    m_timeType->setVisible(true);
}

/**
 * @brief ClockItem::updateDateTime 每一秒更新时间
 */
void ClockItem::updateDateTime()
{
    m_clock->setTimeZone(m_zoneInfo);
    m_clock->update();

    if (!(m_labelTime && m_timeType && m_label && m_labelDate))
        return;
    if (m_bIs24HourType) {
        m_labelTime->setText(QDateTime::currentDateTime().toString(m_longTimeFormat));
        m_timeType->setVisible(false);
    } else {
        translateHourType();
    }

    if (m_bIsEnglishType) {
        m_label->setText(QDate::currentDate().toString("dddd"));
        m_labelDate->setText(QDate::currentDate().toString("MM-dd-yyyy"));
    } else {
        m_label->setText(QDate::currentDate().toString(m_shortDateFormat));
        m_labelDate->setText(QDate::currentDate().toString(m_weekdayFormat));
    }
}

/**
 * @brief ClockItem::setWeekdayFormatType 根据类型设置周的显示格式
 * @param type
 */
void ClockItem::setWeekdayFormatType(int type)
{
    switch (type) {
    case 0: m_weekdayFormat = "dddd";  break;
    case 1: m_weekdayFormat = "ddd"; break;
    default: m_weekdayFormat = "dddd"; break;
    }
    updateDateTime();
}

/**
 * @brief ClockItem::setShortDateFormat 根据类型,设置短日期的显示格式
 * @param type
 */
void ClockItem::setShortDateFormat(int type)
{
    switch (type) {
    case 0: m_shortDateFormat = "yyyy/M/d";  break;
    case 1: m_shortDateFormat = "yyyy-M-d"; break;
    case 2: m_shortDateFormat = "yyyy.M.d"; break;
    case 3: m_shortDateFormat = "yyyy/MM/dd"; break;
    case 4: m_shortDateFormat = "yyyy-MM-dd"; break;
    case 5: m_shortDateFormat = "yyyy.MM.dd"; break;
    case 6: m_shortDateFormat = "yy/M/d"; break;
    case 7: m_shortDateFormat = "yy-M-d"; break;
    case 8: m_shortDateFormat = "yy.M.d"; break;
    default: m_shortDateFormat = "yyyy-MM-dd"; break;
    }
    updateDateTime();
}

/**
 * @brief ClockItem::setLongTimeFormat 根据类型设置长时间的显示格式
 * @param type
 */
void ClockItem::setLongTimeFormat(int type)
{
    switch (type) {
    case 0: m_longTimeFormat = "h:mm:ss"; break;
    case 1: m_longTimeFormat = "hh:mm:ss";  break;
    default: m_longTimeFormat = "hh:mm:ss"; break;
    }
    updateDateTime();
}
