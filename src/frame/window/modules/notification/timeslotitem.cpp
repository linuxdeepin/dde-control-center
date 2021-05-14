/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
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
#include "timeslotitem.h"

#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QTime>

DWIDGET_USE_NAMESPACE
using namespace dcc;
using namespace DCC_NAMESPACE::notification;

//消息通知时间段项
TimeSlotItem::TimeSlotItem(QWidget *parent)
    : SettingsItem(parent)
    , m_chkState(new QCheckBox)
    , m_editStart(new DTimeEdit)
    , m_editEnd(new DTimeEdit)
{
    m_editStart->setDisplayFormat("h:mm");
    m_editStart->setAccessibleName("Start_Time_Edit");

    m_editStart->setTime(QTime(22, 0));

    m_editStart->setFixedWidth(90);
    m_editEnd->setDisplayFormat("h:mm");
    m_editEnd->setAccessibleName("End_Time_Edit");
    m_editEnd->setFixedWidth(90);
    m_editEnd->setTime(QTime(7, 0));

    m_chkState->setAccessibleName("Time_CheckBox");

    m_chkState->setFixedHeight(38);

    QLabel *lblFrom = new QLabel(tr("From"));
    lblFrom->adjustSize();
    QLabel *lblTo = new QLabel(tr("To"));
    lblTo->adjustSize();

    auto layout = new QHBoxLayout;
    layout->setContentsMargins(10, 0, 10, 0);
    layout->addWidget(m_chkState);
    layout->addWidget(lblFrom);
    layout->addWidget(m_editStart);
    layout->addWidget(lblTo);
    layout->addWidget(m_editEnd);
    layout->addStretch();

    setLayout(layout);

    connect(m_chkState, &QCheckBox::stateChanged, this, [ = ]() {
        Q_EMIT stateChanged(getState());
    });
    connect(m_editStart, &DTimeEdit::timeChanged, this, &TimeSlotItem::timeStartChanged);
    connect(m_editEnd, &DTimeEdit::timeChanged, this, &TimeSlotItem::timeEndChanged);
}

bool TimeSlotItem::getState() const
{
    return m_chkState->isChecked();
}

void TimeSlotItem::setState(const bool &state)
{
    if (getState() != state) {
        m_chkState->setChecked(state);
    }
    Q_EMIT stateChanged(state);
}

QTime TimeSlotItem::getTimeStart() const
{
    return m_editStart->time();
}

void TimeSlotItem::setTimeStart(const QTime &time)
{
    if (QTime::fromString(m_editStart->text()) != time) {
        m_editStart->setTime(time);
        Q_EMIT timeStartChanged(time);
    }
}

QTime TimeSlotItem::getTimeEnd() const
{
    return m_editEnd->time();
}

void TimeSlotItem::setTimeEnd(const QTime &time)
{
    if (QTime::fromString(m_editEnd->text()) != time) {
        m_editEnd->setTime(time);
        Q_EMIT timeEndChanged(time);
    }
}
