/*
* Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#ifndef TIMESETTINGMODULE_H
#define TIMESETTINGMODULE_H
#include "interface/moduleobject.h"

DCC_BEGIN_NAMESPACE
class SettingsGroup;
class SwitchWidget;
class ComboxWidget;
class LineEditWidget;
class ButtonTuple;
DCC_END_NAMESPACE

class DatetimeModel;
class DatetimeWorker;

class QSpinBox;
class DateWidget;

class TimeSettingModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit TimeSettingModule(DatetimeModel *model, DatetimeWorker *work, QObject *parent = nullptr);

    void initAutoSyncTime(DCC_NAMESPACE::SettingsGroup *ntpGroup);
    void initTimeSetting(DCC_NAMESPACE::SettingsGroup *datetimeGroup);
    void initDigitalClock(QWidget *w);

    QWidget *extraButton() override;
    virtual void deactive() override;
Q_SIGNALS:
    void requestSetAutoSyncdate(const bool &state);
    void requestSetTime(const QDateTime &time);
    void requestNTPServer(QString server);

public Q_SLOTS:
    void setControlVisible(bool state);
    void onProcessComboBox(const QString &value);
    void onCancelButtonClicked();
    void onConfirmButtonClicked();
    void isUserOperate();

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
