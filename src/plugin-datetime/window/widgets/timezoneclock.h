//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#ifndef TIMEZONECLOCK_H
#define TIMEZONECLOCK_H

#include <QWidget>
#include <QTimeZone>
#include "zoneinfo.h"

class TimezoneClock: public QWidget
{
    Q_OBJECT
public:
    explicit TimezoneClock(QWidget *parent = 0);
    virtual ~TimezoneClock();

    bool drawTicks() const;
    void setDrawTicks(bool drawTicks);

    void setTimeZone(const ZoneInfo &timeZone);

    bool autoNightMode() const;
    void setAutoNightMode(bool autoNightMode);

protected:
    void paintEvent(QPaintEvent *event);

private:
    bool m_drawTicks;
    bool m_autoNightMode;
    ZoneInfo m_timeZone;
};

#endif // TIMEZONECLOCK_H
