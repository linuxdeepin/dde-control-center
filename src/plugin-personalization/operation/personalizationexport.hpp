//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QtQml/qqml.h>

class PersonalizationExport : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    PersonalizationExport(QObject *parent = nullptr)
    : QObject(parent)
    {

    }
    enum WallpaperSetOption {
        Option_Desktop = 0,
        Option_Lock,
        Option_Other,
        Option_All,
    };
    Q_ENUM(WallpaperSetOption)

    enum ModuleType {
        Root = 0,
        Theme,
        Wallpaper,
        ScreenSaver
    };

    Q_ENUM(ModuleType)
};
