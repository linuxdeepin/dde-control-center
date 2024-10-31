//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef UTILS_H
#define UTILS_H

#include <QObject>

const int RENDER_DPI = 72;
const double DPI = 96;

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

#endif // UTILS_H
