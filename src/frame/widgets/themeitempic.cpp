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

#include "widgets/themeitempic.h"

#include <QPainter>
#include <QPainterPath>

using namespace dcc;
using namespace dcc::widgets;

ThemeItemPic::ThemeItemPic(QWidget *parent)
    :QWidget(parent)
{
    setFixedSize(320, 70);
}

void ThemeItemPic::setPicPath(const QString &path)
{
    m_picPath = path;
    update();
}

void ThemeItemPic::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(painter.renderHints() | QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 5, 5);

    painter.save();

    painter.setClipPath(path);
    QPixmap pix(m_picPath);
    painter.drawPixmap(rect(), pix);

    painter.restore();

    painter.end();
}
