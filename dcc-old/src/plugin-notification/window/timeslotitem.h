//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "widgets/settingsitem.h"

#include <DTimeEdit>

QT_BEGIN_NAMESPACE
class QTime;
class QCheckBox;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {

class TimeSlotItem: public SettingsItem
{
    Q_OBJECT
public:
    explicit TimeSlotItem(QWidget *parent = nullptr);

    bool getState()const;
    void setState(const bool &state);

    QTime getTimeStart()const;
    void setTimeStart(const QTime &time);

    QTime getTimeEnd()const;
    void setTimeEnd(const QTime &time);

Q_SIGNALS:
    void stateChanged(bool state);
    void timeStartChanged(QTime time);
    void timeEndChanged(QTime time);

private:
    QCheckBox *m_chkState;
    Dtk::Widget::DTimeEdit *m_editStart;
    Dtk::Widget::DTimeEdit *m_editEnd;
};
}
