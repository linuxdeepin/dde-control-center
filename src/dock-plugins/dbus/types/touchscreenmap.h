// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TOUCHSCREENMAP_H
#define TOUCHSCREENMAP_H

#include <QMap>
#include <QDBusMetaType>

typedef QMap<QString, QString> TouchscreenMap;

void registerTouchscreenMapMetaType();

#endif // TOUCHSCREENMAP_H
