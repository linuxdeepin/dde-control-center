// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
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

DWIDGET_BEGIN_NAMESPACE
class DLineEdit;
DWIDGET_END_NAMESPACE

namespace dcc {
namespace datetime {
class DatetimeModel;
class DateWidget;
class TimeZoneChooser;
}
}

const int SpinBtnLength = 26;

namespace DCC_NAMESPACE {
namespace datetime {
class DateWidget;

class DateSettings : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit DateSettings(QWidget *parent = nullptr);
    virtual ~DateSettings();
    void setCurrentTimeZone(const ZoneInfo &info);
    QDateTime getDatetime() const;
    void setNtpServerAddress(QString address);
    void setLastServerAddress(QString address);

Q_SIGNALS:
    void requestSetTime(const QDateTime &time);
    void requestSetAutoSyncdate(const bool &state);
    void requestBack();
    void notifyBtnClickSetDatetime();
    void requestNTPServer(QString server);

public Q_SLOTS:
    void updateRealAutoSyncCheckState(const bool &state);
    void updateNTPServerList(const QStringList &list);
    void updateSettingformatTime(const bool &value);

private Q_SLOTS:
    void onCancelButtonClicked();
    void onConfirmButtonClicked();
    void updateDayRange();
    void onProcessComboBox(const int &value);
    void isUserOperate();

private:
    QSpinBox *createDSpinBox(QWidget *parent, int min, int max);
    void setControlVisible(bool state);
    void setButtonShowState(bool state);
    void updateSettingTime();
    void updateTime();

private:
    dcc::widgets::SettingsGroup *m_datetimeGroup;
    dcc::widgets::SwitchWidget *m_autoSyncTimeSwitch;
    QSpinBox *m_timeHourWidget;
    QSpinBox *m_timeMinWidget;
    DateWidget *m_yearWidget;
    DateWidget *m_monthWidget;
    DateWidget *m_dayWidget;
    dcc::widgets::ButtonTuple *m_buttonTuple;
    datetimeCombox *m_ntpServerList;
    dcc::widgets::SettingsItem *m_ntpSrvItem;
    dcc::widgets::SettingsItem *m_address;
    DTK_WIDGET_NAMESPACE::DLineEdit *m_addressContent;
    QString m_ntpServerAddress;
    bool m_bIsUserOperate;
    QTimer *m_syncSettingTimer;
    qint64 m_timeSec;
    bool m_Is24HourType;
    QString m_customNtpServer;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
