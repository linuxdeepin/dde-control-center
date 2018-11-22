/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef DATESETTINGS_H
#define DATESETTINGS_H

#include <QWidget>

#include <types/zoneinfo.h>

#include "widgets/contentwidget.h"

class QPushButton;

namespace dcc {
namespace widgets {
class SettingsGroup;
class NextPageWidget;
class ButtonTuple;
}
}

using namespace dcc;
using namespace dcc::widgets;

namespace dcc {
namespace datetime {

class DatetimeModel;
class TimeWidget;
class DateWidget;
class ClockItem;
class TimeZoneChooser;

class DateSettings : public ContentWidget
{
    Q_OBJECT

public:
    explicit DateSettings(QWidget *parent = 0);
    void setCurrentTimeZone(const ZoneInfo &info);

Q_SIGNALS:
    void requestSetTime(const QDateTime &time);

private Q_SLOTS:
    void onCancelButtonClicked();
    void onConfirmButtonClicked();

    void updateDayRange();

private:
    SettingsGroup *m_datetimeGroup;

    ClockItem *m_clock;
    TimeWidget *m_timeWidget;
    DateWidget *m_yearWidget;
    DateWidget *m_monthWidget;
    DateWidget *m_dayWidget;

    dcc::widgets::ButtonTuple *m_buttonTuple;
};

}
}
#endif // DATESETTINGS_H
