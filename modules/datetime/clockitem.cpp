/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "clockitem.h"

#include <QVBoxLayout>
#include <QTimer>

#include "clock.h"
#include "labels/normallabel.h"

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

void ClockItem::updateDateTime()
{
    m_clock->setTimeZone(QTimeZone::systemTimeZone());
    m_clock->update();
    m_label->setText(QDate::currentDate().toString(Qt::SystemLocaleLongDate));
}

} // namespace datetime
} // namespace dcc
