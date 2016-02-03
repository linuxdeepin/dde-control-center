/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "diskicon.h"

DiskIcon::DiskIcon(QWidget *parent) : QLabel(parent)
{
    setFixedSize(48,48);
}

void DiskIcon::setIcon(const QString &icon)
{
    QString tmpUrl = getIconUrl(icon);
    if (tmpUrl != "")
    {
        QPixmap iconMap(tmpUrl);
        setPixmap(iconMap.scaled(size()));
    }
    else
        setPixmap(QIcon::fromTheme(icon).pixmap(size()));
}

void DiskIcon::setMountPoint(const QString &path)
{
    m_mountPoint = path;
}

void DiskIcon::mousePressEvent(QMouseEvent *)
{
    QProcess * tmpProcess = new QProcess();
    connect(tmpProcess, SIGNAL(finished(int)), tmpProcess, SLOT(deleteLater()));
    tmpProcess->start("xdg-open " + m_mountPoint);
}

QString DiskIcon::getIconUrl(const QString &icon)
{
    QString tmpUrl = QString("://Resource/images/%1.png").arg(icon);

    if (QFile::exists(tmpUrl))
        return tmpUrl;
    else
        return "";
}
