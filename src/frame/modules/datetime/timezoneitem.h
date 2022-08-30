// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef TIMEZONEITEM_H
#define TIMEZONEITEM_H

#include "widgets/settingsitem.h"
#include "clock.h"
#include <types/zoneinfo.h>

#include <DIconButton>
#include <DTipLabel>

#include <QLabel>
#include <QString>
#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace datetime {

class TimezoneItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit TimezoneItem(QFrame *parent = nullptr);

    inline ZoneInfo timeZone() const { return m_timezone; }
    void setTimeZone(const ZoneInfo &info);

    void toRemoveMode();
    void toNormalMode();

    void updateInfo();
    void setDetailVisible(bool state);

Q_SIGNALS:
    void removeClicked();

private:
    ZoneInfo m_timezone;

    QLabel *m_city;
    DTipLabel *m_details;
    Clock *m_clock;
    DIconButton *m_removeBtn;
};
}
}
#endif // TIMEZONEITEM_H
