//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "interface/namespace.h"

#include <QWidget>
#include <QTimeZone>
#include "zoneinfo.h"

class Clock : public QWidget
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
    QPixmap getPixmap(const QString &name, const QSize size);

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
