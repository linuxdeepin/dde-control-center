/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "helper.h"
#include <QFile>

#include <libdui/dthememanager.h>

DUI_USE_NAMESPACE

QString readStyleSheet(const QString &widgetName)
{
    return DThemeManager::instance()->getQssForWidget(widgetName);
}

QString getThemeImage(const QString &iconName)
{
    return QString{":/%1/images/%2"}.arg(DThemeManager::instance()->theme(), iconName);
}
