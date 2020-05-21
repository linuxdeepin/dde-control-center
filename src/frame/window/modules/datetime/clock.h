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

#include <QWidget>
#include <QTimeZone>
#include <types/zoneinfo.h>

static const QSize clockSize = QSize(224, 224);
static const QSize pointSize = QSize(145, 15);

namespace DCC_NAMESPACE {
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
    void setPlate(bool isBlack = true);
    QPixmap getPixmap(const QString name, const QSize size);

protected:
    void paintEvent(QPaintEvent *event);

private:
    bool m_drawTicks;
    bool m_autoNightMode;
    bool n_bIsUseBlackPlat;
    bool m_isBlack;
    ZoneInfo m_timeZone;
    QPixmap m_plat;
    QPixmap m_hour;
    QPixmap m_min;
    QPixmap m_sec;
};
}// namespace datetime
}// namespace DCC_NAMESPACE
