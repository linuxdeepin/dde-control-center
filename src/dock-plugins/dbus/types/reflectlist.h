// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef REFLECTLIST_H
#define REFLECTLIST_H

#include <QDBusMetaType>
#include <QList>

typedef QList<quint16> ReflectList;

void registerReflectListMetaType();

#endif // REFLECTLIST_H
