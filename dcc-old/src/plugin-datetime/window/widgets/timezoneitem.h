//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#ifndef TIMEZONEITEM_H
#define TIMEZONEITEM_H

#include "widgets/settingsitem.h"
#include "timezoneclock.h"
#include "zoneinfo.h"

#include <DIconButton>
#include <DTipLabel>

#include <QLabel>
#include <QString>
#include <QHBoxLayout>



class TimezoneItem : public DCC_NAMESPACE::SettingsItem
{
    Q_OBJECT

public:
    explicit TimezoneItem(QFrame *parent = nullptr);

    inline ZoneInfo timeZone() const { return m_timezone; }

    void toRemoveMode();
    void toNormalMode();

    void updateInfo();
    void setDetailVisible(bool state);

Q_SIGNALS:
    void removeClicked();

public Q_SLOTS:
    void setTimeZone(const ZoneInfo &info);

private:
    ZoneInfo m_timezone;

    QLabel *m_city;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_details;
    TimezoneClock *m_clock;
    DTK_WIDGET_NAMESPACE::DIconButton *m_removeBtn;
};

#endif // TIMEZONEITEM_H
