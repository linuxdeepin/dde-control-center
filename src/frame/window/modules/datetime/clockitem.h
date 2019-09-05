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
#pragma once

#include "window/namespace.h"
#include "widgets/settingsitem.h"

#include <QDateTime>
#include <types/zoneinfo.h>

namespace dcc {
namespace widgets {
class NormalLabel;
}
}

namespace DCC_NAMESPACE {
namespace datetime {

class Clock;

class ClockItem : public QWidget
{
    Q_OBJECT
public:
    explicit ClockItem(QWidget *parent = 0, bool isDisplay = true);

    void setTimeZone(const ZoneInfo &zone);
    void setTimeHourType(bool type);
    void setPlate(bool state);

private:
    void translateHourType();

private Q_SLOTS:
    void updateDateTime();

private:
    Clock *m_clock;
    dcc::widgets::NormalLabel *m_label;
    dcc::widgets::NormalLabel *m_labelTime;
    dcc::widgets::NormalLabel *m_labelDate;
    ZoneInfo m_zoneInfo;
    bool m_bIs24HourType;
};

} // namespace datetime
} // namespace DCC_NAMESPACE
