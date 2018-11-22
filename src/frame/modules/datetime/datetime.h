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

#ifndef DATETIME_H
#define DATETIME_H

#include "datesettings.h"
#include "widgets/contentwidget.h"

#include "modules/modulewidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingsitem.h"
#include "widgets/settingshead.h"
#include "widgets/nextpagewidget.h"
#include "widgets/switchwidget.h"
#include "timezoneitem.h"

#include <QSettings>
#include <QPointer>

#include <types/zoneinfo.h>

using namespace dcc;

namespace dcc {
namespace datetime {

class DatetimeModel;
class TimeZoneChooser;

class Datetime : public ModuleWidget
{
    Q_OBJECT

public:
    explicit Datetime();
    ~Datetime();

    void setModel(const DatetimeModel *model);

Q_SIGNALS:
    void editChanged(bool edit);
    void editDatetime();
    void addClick();

    void requestSetNtp(const bool &ntp);
    void requestTimeSettings();
#ifndef DCC_DISABLE_TIMEZONE
    void requestAddUserTimeZone(const QString &zone);
    void requestRemoveUserTimeZone(const ZoneInfo &zone);
    void requestSetTimeZone(const QString &zone);
    void requestHold();
    void requestUnhold();
#endif

public Q_SLOTS:
#ifndef DCC_DISABLE_TIMEZONE
    void addTimezone(const ZoneInfo &zone);
    void addTimezones(const QList<ZoneInfo> &zones);
    void removeTimezone(const ZoneInfo &zone);
    void updateSystemTimezone(const QString &timezone);
#endif
    void updateTimezoneItems();

private Q_SLOTS:
#ifndef DCC_DISABLE_TIMEZONE
    void onEditClicked(const bool &edit);
#endif

private:
    const DatetimeModel *m_model;

    SettingsGroup *m_timeSettingsGroup;
    SwitchWidget *m_ntpSwitch;
    NextPageWidget *m_timePageButton;

#ifndef DCC_DISABLE_TIMEZONE
    SettingsGroup *m_timezoneGroup;
    SettingsHead *m_headItem;
    QPushButton *m_addTimezoneButton;
    NextPageWidget *m_timezoneItem;

    void ensureZoneChooserDialog();
#endif
    ClockItem *m_clockItem;
    QList<TimezoneItem*> m_zoneList;
    bool m_addTimeZone;
    QPointer<TimeZoneChooser> m_dialog;
};
}
}
#endif // DATETIME_H
