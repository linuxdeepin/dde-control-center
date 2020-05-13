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
#pragma once

#include "interface/namespace.h"
#include "widgets/settingsitem.h"

#include <DTimeEdit>

QT_BEGIN_NAMESPACE
class QTime;
class QCheckBox;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace notification {

class TimeSlotItem: public dcc::widgets::SettingsItem
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

}   //namespace notification
}   //namespace dccV20
