// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/settingsitem.h"

#include <com_deepin_daemon_timedate.h>
#include <com_deepin_daemon_appearance.h>

#include <QDateTime>
#include <QHBoxLayout>

#include <types/zoneinfo.h>

using Timedate = com::deepin::daemon::Timedate;
using Appearance = com::deepin::daemon::Appearance;

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
    ~ClockItem();

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
    Appearance *m_appearanceInter;
    QString m_weekdayFormat;
    QString m_shortDateFormat;
    QString m_longTimeFormat;
    QHBoxLayout *m_timeLayout;
    double m_labelTimeFontSize;
};

} // namespace datetime
} // namespace DCC_NAMESPACE
