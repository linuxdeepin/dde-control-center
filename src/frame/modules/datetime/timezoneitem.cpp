/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "timezoneitem.h"
#include "widgets/labels/normallabel.h"

#include <QDebug>
#include <QVBoxLayout>

using namespace dcc::widgets;

namespace dcc {
namespace datetime {

TimezoneItem::TimezoneItem(QFrame *parent)
    : SettingsItem(parent)
    , m_city(new NormalLabel)
    , m_details(new DTipLabel(""))
    , m_clock(new Clock)
    , m_removeBtn(new DIconButton(this))
{
    setFixedHeight(60);

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

    m_removeBtn->setFixedSize(QSize(48, 48));
    m_removeBtn->setIconSize(QSize(24, 24));
    m_removeBtn->setVisible(false);

    m_clock->setDrawTicks(false);
    m_clock->setFixedSize(QSize(48, 48));

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setMargin(0);
    hlayout->setSpacing(0);
    hlayout->setContentsMargins(20, 0, 10, 0);
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
    m_removeBtn->setVisible(false);;
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
        gmData = QString("(GMT+%1:%2)").arg(utcOff, 2, 10, QLatin1Char('0')).arg(m_timezone.getUTCOffset() % 3600 / 60, 2, 10, QLatin1Char('0'));
    } else {
        gmData = QString("(GMT%1:%2)").arg(utcOff, 3, 10, QLatin1Char('0')).arg(m_timezone.getUTCOffset() % 3600 / 60, 2, 10, QLatin1Char('0'));
    }

    m_details->setText(QString("%1, %2").arg(dateLiteral).arg(compareLiteral));
    m_city->setText(m_timezone.getZoneCity() + gmData);
    m_clock->setTimeZone(m_timezone);

    m_removeBtn->setAccessibleName(m_timezone.getZoneCity() + "_DEL");
}

void TimezoneItem::setDetailVisible(bool state)
{
    if (m_details) {
        m_details->setVisible(state);
    }
}

}
}
