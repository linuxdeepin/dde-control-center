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
#ifndef TIMEZONEMODULE_H
#define TIMEZONEMODULE_H
#include "interface/moduleobject.h"

#include "zoneinfo.h"

DCC_BEGIN_NAMESPACE
class SettingsGroup;
DCC_END_NAMESPACE

class DatetimeModel;
class DatetimeWorker;

class TimezoneItem;
class TimeZoneChooser;

class TimezoneModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit TimezoneModule(DatetimeModel *model, DatetimeWorker *work, QObject *parent = nullptr);

    void initTimezoneListGroup(DCC_NAMESPACE::SettingsGroup *timezoneGroup);

Q_SIGNALS:
    void requestRemoveUserTimeZone(const ZoneInfo &zone);
    void notifyItemCount(int);
    void requestAddTimeZone();
    void requestAddUserTimeZone(const QString &zone);
    void requestSetTimeZone(const QString &zone);
    void exitEdit();

public Q_SLOTS:
    void ensureZoneChooserDialog(bool setZone);
    void onEditClicked(const bool &edit);

private:
    DatetimeModel *m_model;
    DatetimeWorker *m_work;

    QList<TimezoneItem *> m_zoneList;
    DCC_NAMESPACE::SettingsGroup *m_timezoneGroup;
};

#endif // TIMEZONEMODULE_H
