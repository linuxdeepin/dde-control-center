//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "timeslotitem.h"

#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QTime>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

//消息通知时间段项
TimeSlotItem::TimeSlotItem(QWidget *parent)
    : SettingsItem(parent)
    , m_chkState(new QCheckBox)
    , m_editStart(new DTimeEdit)
    , m_editEnd(new DTimeEdit)
{
    m_editStart->setDisplayFormat("h:mm");
    m_editStart->setAlignment(Qt::AlignCenter);
    m_editStart->setAccessibleName("Start_Time_Edit");
    m_editStart->setProperty("_d_dtk_spinBox", true);
    m_editEnd->setDisplayFormat("h:mm");
    m_editEnd->setAlignment(Qt::AlignCenter);
    m_editEnd->setAccessibleName("End_Time_Edit");
    m_editEnd->setProperty("_d_dtk_spinBox", true);
    m_chkState->setAccessibleName("Time_CheckBox");
    m_chkState->setMinimumHeight(40);

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
