//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef ROTATIONLIST_H
#define ROTATIONLIST_H

#include <QDBusMetaType>
#include <QList>

typedef QList<quint16> RotationList;

void registerRotationListMetaType();

#endif // ROTATIONLIST_H
