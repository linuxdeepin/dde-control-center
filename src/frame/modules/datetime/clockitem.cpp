/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#include "clockitem.h"

#include <QVBoxLayout>
#include <QTimer>

#include "clock.h"
#include "widgets/labels/normallabel.h"

using namespace dcc::widgets;

namespace dcc {
namespace datetime {

ClockItem::ClockItem(QWidget *parent) :
    SettingsItem(parent),
    m_clock(new Clock),
    m_label(new NormalLabel)
{
    m_clock->setFixedSize(160, 160);
    m_clock->setAutoNightMode(false);
    updateDateTime();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addSpacing(20 * 2);
    layout->addWidget(m_clock, 0, Qt::AlignHCenter);
    layout->addSpacing(20);
    layout->addWidget(m_label, 0, Qt::AlignHCenter);
    layout->addSpacing(20);

    setLayout(layout);

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, &QTimer::timeout, this, &ClockItem::updateDateTime);

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
    m_label->setText(QDate::currentDate().toString(Qt::SystemLocaleLongDate));
}

} // namespace datetime
} // namespace dcc
