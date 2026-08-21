//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QUrl>
#include <QBuffer>
#include <QImage>
#include <QPixmap>

const int RENDER_DPI = 72;
const double DPI = 96;
const int THUMBNAIL_ICON_WIDTH = 84;
const int THUMBNAIL_ICON_HEIGHT = 54;

#define DEEPIN_CUSTOM_SCREENSAVER "deepin-custom-screensaver"

inline QString getGlobalThemeId(const QString &themeId, QString &mode)
{
    QString id = themeId;
    mode.clear();
    if (id.endsWith(".light")) {
        id.chop(6);
        mode = ".light";
    } else if (id.endsWith(".dark")) {
        id.chop(5);
        mode = ".dark";
    }
    return id;
}

inline double ptToPx(double pt)
{
    double px = pt / RENDER_DPI * DPI + 0.5;
    return px;
}

inline double pxToPt(double px)
{
    double pt = px * RENDER_DPI / DPI;
    return pt;
}

inline bool isURI(QString uri)
{
    if (uri.indexOf("://") != -1)
        return true;

    return false;
}


inline QString deCodeURI(QString uri)
{
    QString path;
    if (isURI(uri)) {
        QUrl Url(uri);
        path = Url.path();
    } else {
        path = uri;
    }

    return path;
}

inline QString enCodeURI(QString content, QString scheme)
{
    QString path;
    if (isURI(content)) {
        path = deCodeURI(content);
    } else {
        path = content;
    }
    return scheme + path;
}

inline static QString currentUserName()
{
    static QString cutName = qgetenv("USER");
    return cutName;
}

#endif // UTILS_H
