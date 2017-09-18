/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#include "modulewidgetheader.h"

#include <QPainter>
#include <QMouseEvent>

namespace dcc {

namespace widgets {

ModuleWidgetHeader::ModuleWidgetHeader(QWidget *parent)
    : QFrame(parent)
{
    setCursor(Qt::PointingHandCursor);
    setStyleSheet("dcc--widgets--ModuleWidgetHeader {"
                  "padding: 5px 0;"
                  "}"
                  "dcc--widgets--ModuleWidgetHeader:hover {"
                  "background-color: rgba(255, 255, 255, 0.1);"
                  "border-radius: 4px;"
                  "}");
}

void ModuleWidgetHeader::mouseReleaseEvent(QMouseEvent *e)
{
    QFrame::mouseReleaseEvent(e);

    if (e->button() != Qt::LeftButton)
        return;

    e->accept();

    emit clicked();
}

}

}
