// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QFile>
#include <QFileInfo>
#include <QPainter>
#include <QSvgRenderer>
#include <QIcon>
#include <QApplication>
#include <QScreen>
#include <QUrl>
#include <QDebug>

#include "appicon.h"

AppIcon::AppIcon(QWidget *parent) :
    QLabel(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setAlignment(Qt::AlignCenter);
}

void AppIcon::setIcon(const QString &iconPath, const QString &fallback)
{
    const qreal pixelRatio = qApp->primaryScreen()->devicePixelRatio();
    QPixmap pixmap;

    if (iconPath.startsWith("data:image/")) {
        // iconPath is a string representing an inline image.
        QStringList strs = iconPath.split("base64,");
        if (strs.length() == 2) {
            QByteArray data = QByteArray::fromBase64(strs.at(1).toLatin1());
            pixmap.loadFromData(data);
        }
    }

    if (pixmap.isNull()) {
        QString iconUrl;
        const QUrl url(iconPath);
        iconUrl = url.isLocalFile() ? url.toLocalFile() : url.url();
        const QIcon &icon = QIcon::fromTheme(iconPath, QIcon::fromTheme(fallback, QIcon::fromTheme("application-x-desktop")));
        pixmap = icon.pixmap(width() * pixelRatio, height() * pixelRatio);
    }

    if (!pixmap.isNull()) {
        pixmap = pixmap.scaled(width() * pixelRatio, height() * pixelRatio,
                               Qt::KeepAspectRatioByExpanding,
                               Qt::SmoothTransformation);

        pixmap.setDevicePixelRatio(pixelRatio);
    }
    setPixmap(pixmap);
}
