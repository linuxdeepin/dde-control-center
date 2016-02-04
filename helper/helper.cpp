/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/


#include <gtk/gtk.h>

#include <QFile>
#include <QPixmap>
#include <QDebug>

#include "helper.h"

QString Helper::THEME_NAME = getThemeName();
QSettings Helper::SETTINGS("deepin", "dde-control-center-helper", nullptr);

const QString Helper::searchAppIcon(const QStringList &iconName, int size)
{
    if (iconName.isEmpty())
        return QString();

    const QString cacheKey = QString("%1-%2-%3").arg(iconName.first())
                                                .arg(THEME_NAME)
                                                .arg(size);

    const QVariant value = SETTINGS.value(cacheKey);
    if (value.isValid())
        return value.toString();

    GtkIconInfo *iconInfo;
    GtkIconTheme *iconTheme = gtk_icon_theme_get_default();
    gtk_icon_theme_append_search_path(iconTheme, "/usr/share/pixmaps");

    for (const QString &icon : iconName)
    {
        if (QFile(icon).exists() && !QPixmap(icon).isNull())
            return icon;

        iconInfo = gtk_icon_theme_lookup_icon(iconTheme, icon.toStdString().c_str(), size, GTK_ICON_LOOKUP_GENERIC_FALLBACK);
        if (iconInfo)
            break;
    }

    if (iconInfo) {
        const QString &iconPath = g_strdup(gtk_icon_info_get_filename(iconInfo));

#if GTK_MAJOR_VERSION >= 3
        g_object_unref(iconInfo);
#elif GTK_MAJOR_VERSION == 2
        gtk_icon_info_free(iconInfo);
#endif

        SETTINGS.setValue(cacheKey, iconPath);

        return iconPath;
    }

    qWarning() << iconName << " - icon not found";
    return QString();
}

void Helper::refreshThemeInfo()
{
    THEME_NAME = getThemeName();
}

const QString Helper::getThemeName()
{
    char *themeName;
    g_object_get(gtk_settings_get_default(), "gtk-icon-theme-name", &themeName, NULL);

    return QString(themeName);
}
