/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef APPICON_H
#define APPICON_H

#include <QLabel>
#include <QPixmap>

class AppIcon : public QLabel
{
    Q_OBJECT
public:
    explicit AppIcon(QWidget *parent = 0);

    void setIcon(const QString &iconPath);

private:
    QString getThemeIconPath(QString iconName);
    void updateIcon();
};

#endif // APPICON_H
