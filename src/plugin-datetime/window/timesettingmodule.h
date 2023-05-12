//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TIMESETTINGMODULE_H
#define TIMESETTINGMODULE_H
#include "interface/pagemodule.h"

namespace DCC_NAMESPACE {
class SettingsGroup;
class SwitchWidget;
class ComboxWidget;
class LineEditWidget;
class ButtonTuple;
}

class DatetimeModel;
class DatetimeWorker;

class QSpinBox;
class DateWidget;

class TimeSettingModule : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT
public:
    explicit TimeSettingModule(DatetimeModel *model, DatetimeWorker *work, QObject *parent = nullptr);

    void initAutoSyncTime(DCC_NAMESPACE::SettingsGroup *ntpGroup);
    void initTimeSetting(DCC_NAMESPACE::SettingsGroup *datetimeGroup);
    void initDigitalClock(QWidget *w);

    virtual void deactive() override;
Q_SIGNALS:
    void requestSetAutoSyncdate(const bool &state);
    void requestSetTime(const QDateTime &time);
    void requestNTPServer(QString server);

public Q_SLOTS:
    void setControlVisible(bool state);
    void onCancelButtonClicked();
    void onConfirmButtonClicked();
    void isUserOperate();
    void setBtnEnable(bool state);

private:
    QSpinBox *createDSpinBox(QWidget *parent, int min, int max);
    void setButtonShowState(bool state);

private:
    DatetimeModel *m_model;
    DatetimeWorker *m_work;

    DCC_NAMESPACE::SwitchWidget *m_autoSyncTimeSwitch;
    DCC_NAMESPACE::SettingsGroup *m_datetimeGroup;
    QSpinBox *m_timeHourWidget;
    QSpinBox *m_timeMinWidget;
    DateWidget *m_yearWidget;
    DateWidget *m_monthWidget;
    DateWidget *m_dayWidget;
    DCC_NAMESPACE::ButtonTuple *m_buttonTuple;

    DCC_NAMESPACE::ComboxWidget *m_ntpServerList;
    DCC_NAMESPACE::LineEditWidget *m_customizeAddress;
    //    dcc::widgets::SettingsItem *m_ntpSrvItem;
    //    dcc::widgets::SettingsItem *m_address;
    bool m_bIsUserOperate;
    QString m_customNtpServer;
};

#endif // TIMESETTINGMODULE_H
