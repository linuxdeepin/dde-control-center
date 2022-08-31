// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
class MainWindow;
namespace datetime {
class TimezoneList;
class DatetimeWidget;
class DateSettings;
class FormatSetting;
class CurrencyFormat;
class NumberFormat;

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

    void preInitialize(bool sync = false , FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    virtual void initialize() override;
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    virtual void active() override;
    virtual int load(const QString &path) override;
    QStringList availPage() const override;
    virtual void addChildPageTrans() const override;

private:
    void initSearchData() override;

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
    DCC_NAMESPACE::datetime::FormatSetting *m_fsetting;
    CurrencyFormat *m_currencyFormatWidget;
    NumberFormat *m_numberFormatWidget;
    MainWindow *m_pMainWindow;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
