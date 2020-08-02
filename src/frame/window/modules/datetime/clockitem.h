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

#include "interface/namespace.h"
#include "widgets/settingsitem.h"

#include <com_deepin_daemon_timedate.h>

#include <QDateTime>

#include <types/zoneinfo.h>

using Timedate = com::deepin::daemon::Timedate;

namespace dcc {
namespace widgets {
class NormalLabel;
}
}

DWIDGET_BEGIN_NAMESPACE
class DTipLabel;
DWIDGET_END_NAMESPACE


namespace DCC_NAMESPACE {
namespace datetime {

class Clock;

class ClockItem : public QWidget
{
    Q_OBJECT
public:
    explicit ClockItem(QWidget *parent = nullptr, bool isDisplay = true);

    void setTimeZone(const ZoneInfo &zone);
    void setTimeHourType(bool type);
    void setTimeEnglishType(bool type);
    void setPlate(bool state);

private:
    void translateHourType();

private Q_SLOTS:
    void updateDateTime();
    void setWeekdayFormatType(int type);
    void setShortDateFormat(int type);
    void setLongTimeFormat(int type);

private:
    Clock *m_clock;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_label;
    dcc::widgets::NormalLabel *m_labelTime;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_labelDate;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_timeType;
    ZoneInfo m_zoneInfo;
    bool m_bIs24HourType;
    bool m_bIsEnglishType;
    bool m_weekStartMonType;
    Timedate *m_timedateInter;
    QString m_weekdayFormat;
    QString m_shortDateFormat;
    QString m_longTimeFormat;
};

} // namespace datetime
} // namespace DCC_NAMESPACE
