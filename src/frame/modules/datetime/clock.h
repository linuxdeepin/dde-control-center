// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef CLOCK_H
#define CLOCK_H

#include <QWidget>
#include <QTimeZone>
#include <types/zoneinfo.h>

namespace dcc {
namespace datetime {

class Clock: public QWidget
{
    Q_OBJECT
public:
    explicit Clock(QWidget *parent = 0);
    virtual ~Clock();

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
}
}
#endif // CLOCK_H
