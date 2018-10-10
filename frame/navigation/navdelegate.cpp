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
#include <QScreen>

NavDelegate::NavDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void NavDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool isHover = index.data(NavModel::NavHoverRole).toBool();

    QRect rect = QRect(option.rect.left() + 10,
                       option.rect.top() + 10,
                       option.rect.width() - 10,
                       option.rect.height() - 10);

    // draw background
    if (isHover) {
        painter->fillRect(rect, QColor(255, 255, 255, 25));
    } else {
        painter->fillRect(rect, QColor(255, 255, 255, 7));
    }

    QString moduleName = index.data(Qt::WhatsThisRole).toString();
    if (!moduleName.isEmpty()) {
        QPixmap modulePm = loadPixmap(QString(":/icons/nav_%1.png").arg(moduleName));

        // Keep and offset from the top left corner, base is 1080P
        const double Sh = qApp->primaryScreen()->geometry().height();
        double keepRatio = 1;
        if (Sh <= 1080) {
            keepRatio = Sh / 1080;
        }

        QPoint p(rect.x() + 20 * keepRatio, rect.y() + 26 * keepRatio);
        painter->drawPixmap(p, modulePm);

        if (rect.height() <= modulePm.height() * 2.5) {
            p = QPoint(p.x() + modulePm.width() + 20 * keepRatio, p.y());
        }
        else {
            p = QPoint(p.x(), p.y() + modulePm.height());
        }

        QTextOption option;
        option.setAlignment(Qt::AlignLeft | Qt::AlignTop);

        painter->drawText(QRect(p, QSize(rect.width() - 20 * keepRatio, rect.height())),
                          index.data(NavModel::NavDisplayRole).toString(),
                          option);
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
