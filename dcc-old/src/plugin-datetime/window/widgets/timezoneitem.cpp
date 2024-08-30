//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#include "timezoneitem.h"

#include <QDebug>
#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

TimezoneItem::TimezoneItem(QFrame *parent)
    : SettingsItem(parent)
    , m_city(new QLabel)
    , m_details(new DTipLabel(""))
    , m_clock(new TimezoneClock)
    , m_removeBtn(new DIconButton(this))
{
    m_clock->setAccessibleName("TimezoneItem_clock");
    addBackground();
    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setMargin(0);
    vlayout->setSpacing(0);

    m_city->setObjectName("DCC-Datetime-TimezoneItem-Label");
    m_details->setObjectName("DCC-Datetime-TimezoneItem-Label");
    m_details->setAlignment(Qt::AlignLeft);

    vlayout->addWidget(m_city);
    vlayout->addSpacing(1);
    vlayout->addWidget(m_details);

    m_removeBtn->setFlat(true);
    m_removeBtn->setIcon(DStyle::StandardPixmap::SP_DeleteButton);
    m_removeBtn->setObjectName("remove_button");

    m_removeBtn->setFixedSize(QSize(48, 48));
    m_removeBtn->setIconSize(QSize(24, 24));
    m_removeBtn->setVisible(false);

    m_clock->setDrawTicks(false);
    m_clock->setFixedSize(QSize(48, 48));

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setMargin(0);
    hlayout->setSpacing(0);
    hlayout->setContentsMargins(14, 0, 10, 0); // TODO: 设计沟通设置 14
    hlayout->addLayout(vlayout);
    hlayout->addStretch();
    hlayout->addWidget(m_clock, 0, Qt::AlignVCenter);
    hlayout->addWidget(m_removeBtn);
    setLayout(hlayout);

    connect(m_removeBtn, &DIconButton::clicked, this, &TimezoneItem::removeClicked);
}

void TimezoneItem::setTimeZone(const ZoneInfo &info)
{
    m_timezone = info;

    updateInfo();
}

void TimezoneItem::toRemoveMode()
{
    m_clock->setVisible(false);
    m_removeBtn->setVisible(true);
}

void TimezoneItem::toNormalMode()
{
    m_removeBtn->setVisible(false);
    ;
    m_clock->setVisible(true);
}

void TimezoneItem::updateInfo()
{
    const QDateTime localTime(QDateTime::currentDateTime());

    const double timeDelta = (m_timezone.getUTCOffset() - localTime.offsetFromUtc()) / 3600.0;

    QString dateLiteral;
    if (localTime.time().hour() + timeDelta >= 24) {
        dateLiteral = tr("Tomorrow");
    } else if (localTime.time().hour() + timeDelta <= 0) {
        dateLiteral = tr("Yesterday");
    } else {
        dateLiteral = tr("Today");
    }

    int decimalNumber = 1;
    //小时取余,再取分钟,将15分钟的双倍只显示一位小数,其他的都显示两位小数
    switch ((m_timezone.getUTCOffset() - localTime.offsetFromUtc()) % 3600 / 60 / 15) {
    case -1:
    case -3:
    case 1:
    case 3:
        decimalNumber = 2;
        break;
    default:
        decimalNumber = 1;
        break;
    }

    QString compareLiteral;
    if (timeDelta > 0) {
        compareLiteral = tr("%1 hours earlier than local").arg(QString::number(timeDelta, 'f', decimalNumber));
    } else {
        compareLiteral = tr("%1 hours later than local").arg(QString::number(-timeDelta, 'f', decimalNumber));
    }

    QString gmData = "";
    int utcOff = m_timezone.getUTCOffset() / 3600;
    if (utcOff >= 0) {
        gmData = QString("(UTC+%1:%2)").arg(utcOff, 2, 10, QLatin1Char('0')).arg(m_timezone.getUTCOffset() % 3600 / 60, 2, 10, QLatin1Char('0'));
    } else {
        gmData = QString("(UTC%1:%2)").arg(utcOff, 3, 10, QLatin1Char('0')).arg(m_timezone.getUTCOffset() % 3600 / 60, 2, 10, QLatin1Char('0'));
    }

    m_details->setText(QString("%1, %2").arg(dateLiteral).arg(compareLiteral));
    QString cityName = m_timezone.getZoneCity().isEmpty() ? m_timezone.getZoneName() : m_timezone.getZoneCity();
    m_city->setText(cityName + gmData);
    m_clock->setTimeZone(m_timezone);

    m_removeBtn->setAccessibleName(m_timezone.getZoneCity() + "_DEL");
}

void TimezoneItem::setDetailVisible(bool state)
{
    if (m_details) {
        m_details->setVisible(state);
    }
}
