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

#include "interface/namespace.h"
#include "timezonecontentlist.h"

#include <QWidget>
#include <QVBoxLayout>

#include <types/zoneinfo.h>
#include <DFloatingButton>

namespace dcc {
namespace datetime {
class TimeZoneChooser;
class TimezoneItem;
}

namespace widgets {
class SettingsGroup;
class SettingsItem;
class SettingsHead;
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

    TimezoneContentList *getTimezoneContentListPtr();

private:
    QVBoxLayout *m_centralLayout;
    DTK_WIDGET_NAMESPACE::DFloatingButton *m_addTimezoneButton;
    dcc::widgets::SettingsGroup *m_headGroup;
    dcc::widgets::SettingsHead *m_headItem;
    TimezoneContentList *m_contentList;

Q_SIGNALS:
    void requestAddUserTimeZone(const QString &zone);
    void requestAddTimeZone();

public Q_SLOTS:
};

}// namespace datetime
}// namespace DCC_NAMESPACE
