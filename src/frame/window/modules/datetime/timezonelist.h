// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
