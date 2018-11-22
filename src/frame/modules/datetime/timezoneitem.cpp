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

#include <QDebug>
#include <QVBoxLayout>

#include "widgets/labels/normallabel.h"
#include "widgets/labels/tipslabel.h"

using namespace dcc::widgets;

namespace dcc {
namespace datetime {

TimezoneItem::TimezoneItem(QFrame *parent)
    :SettingsItem(parent),
      m_city(new NormalLabel),
      m_details(new TipsLabel),
      m_clock(new Clock),
      m_removeBtn(new DImageButton)
{
    setFixedHeight(60);

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setMargin(0);
    vlayout->setSpacing(0);

    m_city->setObjectName("DCC-Datetime-TimezoneItem-Label");
    m_details->setObjectName("DCC-Datetime-TimezoneItem-Label");

    vlayout->addStretch();
    vlayout->addWidget(m_city);
    vlayout->addSpacing(1);
    vlayout->addWidget(m_details);
    vlayout->addStretch();

    m_removeBtn->setObjectName("DCC-Datetime-TimezoneItem-Remove");
    m_removeBtn->setVisible(false);

    m_clock->setDrawTicks(false);
    m_clock->setFixedSize(QSize(48,48));

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->setMargin(0);
    hlayout->setSpacing(0);
    hlayout->setContentsMargins(20, 0, 10, 0);
    hlayout->addLayout(vlayout);
    hlayout->addStretch();
    hlayout->addWidget(m_clock, 0, Qt::AlignVCenter);
    hlayout->addWidget(m_removeBtn);
    setLayout(hlayout);

    connect(m_removeBtn, &DImageButton::clicked, this, &TimezoneItem::removeClicked);
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
    const QDateTime localTime( QDateTime::currentDateTime() );

    const double timeDelta = (m_timezone.getUTCOffset() - localTime.offsetFromUtc()) / 3600.0;

    QString dateLiteral;
    if (localTime.time().hour() + timeDelta >= 24) {
        dateLiteral = tr("Tomorrow");
    } else if (localTime.time().hour() + timeDelta <= 0) {
        dateLiteral = tr("Yesterday");
    } else {
        dateLiteral = tr("Today");
    }

    QString compareLiteral;
    if(timeDelta > 0) {
        compareLiteral = tr("%1 hours earlier than local").arg(QString::number(timeDelta, 'f', 1));
    } else {
        compareLiteral = tr("%1 hours late than local").arg(QString::number(-timeDelta, 'f', 1));
    }

    m_details->setText(QString("%1, %2").arg(dateLiteral).arg(compareLiteral));
    m_city->setText(m_timezone.getZoneCity());
    m_clock->setTimeZone(m_timezone);
}

}
}
