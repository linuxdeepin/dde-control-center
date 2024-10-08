//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UTILS_H
#define UTILS_H

#include <QObject>

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

#endif // UTILS_H
