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

#include "interface/moduleinterface.h"

#include <QObject>

#include <types/zoneinfo.h>

namespace dcc {
namespace datetime {
class DatetimeWork;
class DatetimeModel;
class TimeZoneChooser;
}
}

namespace DCC_NAMESPACE {
namespace datetime {
class TimezoneList;
class DatetimeWidget;
class DateSettings;

class DatetimeModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    enum DatetimeType {
        Default = -1,
        ETimezoneList,
        TimeSetting,
        FormatSetting,
        ESystemTimezone,
        AddTimeZone,
        Count
    };

public:
    DatetimeModule(FrameProxyInterface *frameProxy, QObject *parent = nullptr);

    virtual void initialize() override;
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    virtual void active() override;
    virtual int load(const QString &path) override;
    QStringList availPage() const override;

Q_SIGNALS:
    void requestAddUserTimeZone(const QString &zone);
    void requestRemoveUserTimeZone(const ZoneInfo &zone);
    void requestSetTimeZone(const QString &zone);
    void weekdayFormatChanged(int index);
    void shortDateFormatChanged(int index);
    void longDateFormatChanged(int index);
    void longTimeFormatChanged(int index);
    void shortTimeFormatChanged(int index);
    void weekStartDayFormatChanged(int index);

public Q_SLOTS:
    void onPopWidget();
    void updateSystemTimezone(const QString &timezone);
    void showSystemTimezone();
    void closeDialog();

    void showTimezoneList();
    void showTimeSetting();
    void ensureZoneChooserDialog();
    void showFormatSetting();

private:
    dcc::datetime::DatetimeWork *m_work;
    dcc::datetime::DatetimeModel *m_model;
    QPointer<dcc::datetime::TimeZoneChooser> m_dialog;
    QPointer<TimezoneList> m_timezonelist;//timezonelist in other page, need exchangle data from this page
    QPointer<DatetimeWidget> m_widget;
    QPointer<DateSettings> m_setting;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
