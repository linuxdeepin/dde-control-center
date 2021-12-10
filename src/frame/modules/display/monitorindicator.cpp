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

#include "monitorindicator.h"

#include <QPainter>

using namespace dcc;
using namespace dcc::display;

MonitorIndicator::MonitorIndicator(QWidget *parent)
    : QFrame(nullptr)
{
    Q_UNUSED(parent)

    setWindowFlags(Qt::CoverWindow | Qt::WindowStaysOnTopHint | Qt::SplashScreen | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    setAttribute(Qt::WA_TranslucentBackground);
}


void MonitorIndicator::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    QRect rect = QRect(0,0,this->geometry().width(),this->geometry().height());
    QPen pen;
    pen.setWidth(20);
    pen.setColor(QColor("#2ca7f8"));
    p.setPen(pen);
    p.setBrush(Qt::BrushStyle::NoBrush);
    p.drawRect(rect);
    QFrame::paintEvent(e);
}
