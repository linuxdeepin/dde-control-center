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
#include <QPainterPath>
#include <QResizeEvent>
#include <QX11Info>

#include <X11/extensions/shape.h>
#include <X11/Xregion.h>

using namespace dcc;
using namespace dcc::display;

MonitorIndicator::MonitorIndicator(QWidget *parent)
    : QFrame(parent)
{
    setWindowFlags(Qt::SplashScreen | Qt::X11BypassWindowManagerHint);
    setStyleSheet("background-color: #2ca7f8;");
}

void MonitorIndicator::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);

    XRectangle rectangle;
    rectangle.x = 0;
    rectangle.y = 0;
    rectangle.width = e->size().width();
    rectangle.height = e->size().height();

    // need to restore the cut area, if not,cut out will be repeated.
    XShapeCombineRectangles(QX11Info::display(), winId(), ShapeBounding, 0, 0, &rectangle, 1, ShapeSet, YXBanded);

    rectangle.x = 10;
    rectangle.y = 10;
    rectangle.width = e->size().width() - 20;
    rectangle.height = e->size().height() - 20;

    XShapeCombineRectangles(QX11Info::display(), winId(), ShapeBounding, 0, 0, &rectangle, 1, ShapeSubtract, YXBanded);
}
