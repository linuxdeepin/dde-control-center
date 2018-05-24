/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#include "navdelegate.h"
#include "navmodel.h"

#include <QPainter>
#include <QImageReader>
#include <QApplication>

NavDelegate::NavDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void NavDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool isHover = index.data(NavModel::NavHoverRole).toBool();

    QRect rect = QRect(option.rect.left(), option.rect.top(), option.rect.width()-1, option.rect.height()-1);

    // draw background
    if (isHover) {
        painter->fillRect(rect, QColor(255, 255, 255, 25));
    } else {
        painter->fillRect(rect, QColor(255, 255, 255, 7));
    }

    QString moduleName = index.data(Qt::WhatsThisRole).toString();
    if (!moduleName.isEmpty()) {
        QPixmap modulePm = loadPixmap(QString(":/icons/nav_%1.png").arg(moduleName));
        const qreal devicePixelRatio = qApp->devicePixelRatio();
        painter->drawPixmap((option.rect.center() - modulePm.rect().center() / devicePixelRatio), modulePm);
    }

    QStyledItemDelegate::paint(painter, option, index);
}

QPixmap NavDelegate::loadPixmap(const QString &path) const
{
    qreal ratio = 1.0;
    QPixmap pixmap;

    const qreal devicePR = qApp->devicePixelRatio();

    if (!qFuzzyCompare(ratio, devicePR)) {
        QImageReader reader;
        reader.setFileName(qt_findAtNxFile(path, devicePR, &ratio));
        if (reader.canRead()) {
            reader.setScaledSize(reader.size() * (devicePR / ratio));
            pixmap = QPixmap::fromImage(reader.read());
            pixmap.setDevicePixelRatio(devicePR);
        }
    } else {
        pixmap.load(path);
    }

    return pixmap;
}
