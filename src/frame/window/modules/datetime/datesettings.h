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
#include "widgets/contentwidget.h"
#include "datetimecombox.h"

#include <types/zoneinfo.h>

#include <QWidget>
#include <QSpinBox>

namespace dcc {
namespace widgets {
class SettingsGroup;
class SettingsItem;
class NextPageWidget;
class ButtonTuple;
class SwitchWidget;
}
}

namespace dcc {
namespace datetime {
class DatetimeModel;
class DateWidget;
class TimeZoneChooser;
}
}

namespace DCC_NAMESPACE {
namespace datetime {

class ClockItem;
class DateWidget;

class DateSettings : public QWidget
{
    Q_OBJECT
public:
    explicit DateSettings(QWidget *parent = nullptr);
    void setCurrentTimeZone(const ZoneInfo &info);
    QDateTime getDatetime() const;
    void setNtpServerAddress(QString address);

Q_SIGNALS:
    void requestSetTime(const QDateTime &time);
    void requestSetAutoSyncdate(const bool &state);
    void requestBack();
    void notifyBtnClickSetDatetime();
    void requestNTPServer(QString server);

public Q_SLOTS:
    void updateRealAutoSyncCheckState(const bool &state);
    void updateNTPServerList(const QStringList list);

private Q_SLOTS:
    void onCancelButtonClicked();
    void onConfirmButtonClicked();
    void updateDayRange();
    void onProcessComboBox(const int &value);
    void isUserOperate();

private:
    QSpinBox *createDSpinBox(QWidget *parent, int min, int max);

private:
    dcc::widgets::SettingsGroup *m_datetimeGroup;
    ClockItem *m_clock;
    dcc::widgets::SwitchWidget *m_autoSyncTimeSwitch;
    QSpinBox *m_timeHourWidget;
    QSpinBox *m_timeMinWidget;
    DateWidget *m_yearWidget;
    DateWidget *m_monthWidget;
    DateWidget *m_dayWidget;
    dcc::widgets::ButtonTuple *m_buttonTuple;
    bool m_bIsConfirmSetTime;
    datetimeCombox *m_ntpServerList;
    dcc::widgets::SettingsItem *m_ntpSrvItem;
    dcc::widgets::SettingsItem *m_address;
    QLineEdit *m_addressContent;
    QString m_ntpServerAddress;
    bool m_bIsUserOperate;
    bool m_bSystemIsServer;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
