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
