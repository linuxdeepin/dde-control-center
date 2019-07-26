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

#include <types/zoneinfo.h>
#include <dimagebutton.h>
#include <QWidget>
#include <QVBoxLayout>

namespace dcc {

namespace datetime {
class DatetimeModel;
class TimeZoneChooser;
class TimezoneItem;
}

namespace widgets {
class SettingsGroup;
class SettingsItem;
}
}

using namespace dcc;
using namespace dcc::datetime;
using namespace dcc::widgets;

namespace DCC_NAMESPACE {
namespace datetime {

class TimezoneContentList : public ContentWidget
{
    Q_OBJECT
public:
    explicit TimezoneContentList(QWidget *parent = nullptr);
    virtual ~TimezoneContentList();

private:
    QVBoxLayout *m_centralLayout;
    SettingsGroup *m_timezoneGroup;
    QList<TimezoneItem *> m_zoneList;

Q_SIGNALS:
    void requestRemoveUserTimeZone(const ZoneInfo &zone);

public Q_SLOTS:
    void addTimezone(const ZoneInfo &zone);
    void addTimezones(const QList<ZoneInfo> &zones);
    void removeTimezone(const ZoneInfo &zone);
    void updateTimezones(const QList<ZoneInfo> &zones);
    void updateTimezoneItems();
    void onEditClicked(const bool &edit);
};

}// namespace datetime
}// namespace DCC_NAMESPACE
