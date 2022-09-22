// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "imageutil.h"

#include <QIcon>
#include <QPainter>

const QPixmap ImageUtil::loadSvg(const QString &iconName, const QString &localPath, const int size, const qreal ratio)
{
    QIcon icon = QIcon::fromTheme(iconName);
    if (!icon.isNull()) {
        QPixmap pixmap = icon.pixmap(QCoreApplication::testAttribute(Qt::AA_UseHighDpiPixmaps) ? size : int(size * ratio));
        pixmap.setDevicePixelRatio(ratio);
        return pixmap;
    }

    QPixmap pixmap(int(size * ratio), int(size * ratio));
    QString localIcon = QString("%1%2%3").arg(localPath).arg(iconName).arg(iconName.contains(".svg") ? "" : ".svg");
    QSvgRenderer renderer(localIcon);
    pixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pixmap);
    renderer.render(&painter);
    painter.end();
    pixmap.setDevicePixelRatio(ratio);

    return pixmap;
}

const QPixmap ImageUtil::loadSvg(const QString &iconName, const QSize size, const qreal ratio)
{
    QIcon icon = QIcon::fromTheme(iconName);
    if (!icon.isNull()) {
        QPixmap pixmap = icon.pixmap(QCoreApplication::testAttribute(Qt::AA_UseHighDpiPixmaps) ? size : size * ratio);
        pixmap.setDevicePixelRatio(ratio);
        return pixmap;
    }
    return QPixmap();
}
