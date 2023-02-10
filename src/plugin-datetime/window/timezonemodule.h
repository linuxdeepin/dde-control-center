//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef TIMEZONEMODULE_H
#define TIMEZONEMODULE_H
#include "interface/pagemodule.h"

#include "zoneinfo.h"

namespace DCC_NAMESPACE {
class SettingsGroup;
}

class DatetimeModel;
class DatetimeWorker;

class TimezoneItem;
class TimeZoneChooser;

class TimezoneModule : public DCC_NAMESPACE::PageModule
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
