/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef HELPER_H
#define HELPER_H

#include <QStringList>
#include <QSettings>

class Helper
{
public:
    static const QString searchAppIcon(const QStringList &iconName, int size);
    static void refreshThemeInfo();

private:
    static const QString getThemeName();

private:
    static QString THEME_NAME;
    static QSettings SETTINGS;
};

#endif // HELPER_H
