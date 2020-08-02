/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     niecheng <niecheng@uniontech.com>
*
* Maintainer: niecheng <niecheng@uniontech.com>
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

#include <types/zoneinfo.h>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {
namespace datetime {
class DatetimeModel;
}
}
namespace dcc {
namespace widgets {
class ComboxWidget;
}
}
namespace DCC_NAMESPACE {
namespace datetime {

class FormatSetting : public QWidget
{
    Q_OBJECT
public:
    explicit FormatSetting(dcc::datetime::DatetimeModel *mdoel, QWidget *parent = nullptr);

Q_SIGNALS:
    void weekdayFormatChanged(int index);
    void shortDateFormatChanged(int index);
    void longDateFormatChanged(int index);
    void longTimeFormatChanged(int index);
    void shortTimeFormatChanged(int index);
    void weekStartDayFormatChanged(int index);
public Q_SLOTS:
    void setCururentWeekdayFormat(int type);
    void setCururentShortDateFormat(int type);
    void setCururentLongDateFormat(int type);
    void setCururentLongTimeFormat(int type);
    void setCururentShortTimeFormat(int type);
    void setCururentWeekStartDayFormat(int type);
private:
    void initComboxWidgetList();
    QString fotmatWeek(int type);
    QString fotmatLongDate(int type);
    QString fotmatShortDate(int type);
    QString fotmatLongTime(int type);
    QString fotmatShortTime(int type);
    QString weekStartWithDay(int type);
    QVBoxLayout *m_layout;
    dcc::datetime::DatetimeModel *mModel;
    dcc::widgets::ComboxWidget *m_weekCbx = nullptr ;
    dcc::widgets::ComboxWidget *m_longdateCbx = nullptr ;
    dcc::widgets::ComboxWidget *m_shortDateCbx = nullptr ;
    dcc::widgets::ComboxWidget *m_longtimeCbx = nullptr;
    dcc::widgets::ComboxWidget *m_shortimeCbx = nullptr ;
    dcc::widgets::ComboxWidget *m_weekStartDayCbx = nullptr ;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
