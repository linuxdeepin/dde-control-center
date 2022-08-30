// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/contentwidget.h"

#include <QWidget>
#include <QVBoxLayout>

#include <types/zoneinfo.h>
#include <dimagebutton.h>

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

namespace DCC_NAMESPACE {
namespace datetime {

class TimezoneContentList : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit TimezoneContentList(QWidget *parent = nullptr);
    virtual ~TimezoneContentList();

    void setSpace(int value);

private:
    QVBoxLayout *m_centralLayout;
    dcc::widgets::SettingsGroup *m_timezoneGroup;
    QList<dcc::datetime::TimezoneItem *> m_zoneList;

Q_SIGNALS:
    void requestRemoveUserTimeZone(const ZoneInfo &zone);
    void notifyItemCount(int);

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
