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

#include "navitemwidget.h"

#include <QPainter>
#include <QApplication>
#include <QIcon>
#include <QDebug>
#include <QImageReader>
#include <QFile>

NavItemWidget::NavItemWidget(const QString &id, QWidget *parent)
    : QFrame(parent),

      m_id(id)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setObjectName("NavItemWidget");
    setStyleSheet("QFrame#NavItemWidget {"
                  "background-color: rgba(255, 255, 255, .03);"
                  "}"
                  ""
                  "QFrame#NavItemWidget:hover {"
                  "background-color: rgba(255, 255, 255, .1);"
                  "}");
}

void NavItemWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    QPainter painter(this);
    const QString &file = QString(":/icons/nav_%1.png").arg(m_id);

    qreal ratio = 1.0;
    const qreal devicePixelRatio = qApp->devicePixelRatio();

    QRect pixRect;
    QPixmap pixmap;

    if (!qFuzzyCompare(ratio, devicePixelRatio)) {
         QImageReader reader;
         reader.setFileName(qt_findAtNxFile(file, devicePixelRatio, &ratio));
         if (reader.canRead()) {
             reader.setScaledSize(reader.size() * (devicePixelRatio / ratio));
             pixmap = QPixmap::fromImage(reader.read());
             pixmap.setDevicePixelRatio(devicePixelRatio);
             pixRect = QRect(0, 0, pixmap.width() / devicePixelRatio, pixmap.height() / devicePixelRatio);
         }
     }

    if (pixmap.isNull()) {
         pixmap.load(file);
         pixRect = pixmap.rect();
     }

    pixRect.moveCenter(rect().center());

    painter.drawPixmap(pixRect, pixmap);
}

void NavItemWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);

    emit itemClicked(m_id);
}

void NavItemWidget::enterEvent(QEvent *e)
{
    QWidget::enterEvent(e);

    emit itemEntered(m_id);

    update();
}
