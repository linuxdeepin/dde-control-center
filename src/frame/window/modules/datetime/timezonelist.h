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
#include "widgets/settingsgroup.h"
#include "widgets/settingsitem.h"
#include "widgets/settingshead.h"
#include "widgets/nextpagewidget.h"

#include <QWidget>
#include <QVBoxLayout>

using namespace dcc::widgets;

namespace dcc {
namespace datetime {
class DatetimeModel;
class TimeZoneChooser;
}
}

namespace DCC_NAMESPACE {
namespace datetime {

class TimezoneList : public QWidget
{
public:
    Q_OBJECT
public:
    explicit TimezoneList(QWidget *parent = nullptr);
    virtual ~TimezoneList();

private:
    QVBoxLayout *m_centralLayout;

#ifndef DCC_DISABLE_TIMEZONE
    SettingsGroup *m_timezoneGroup;
    SettingsHead *m_headItem;
    QPushButton *m_addTimezoneButton;
    NextPageWidget *m_timezoneItem;

    void ensureZoneChooserDialog();
#endif
//    QList<TimezoneItem*> m_zoneList;
    bool m_addTimeZone;
//    QPointer<TimeZoneChooser> m_dialog;

Q_SIGNALS:


public Q_SLOTS:

};

}// namespace datetime
}// namespace DCC_NAMESPACE
