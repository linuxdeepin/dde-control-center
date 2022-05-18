/*
 * Copyright (C) 2016 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INTSTRING_H
#define INTSTRING_H

#include <QtCore/QString>
#include <QtDBus/QtDBus>
#include <QDBusMetaType>

struct IntString
{
    qint32 state;
    QString description;

    bool operator!=(const IntString &intString);
};

Q_DECLARE_METATYPE(IntString)

QDBusArgument &operator<<(QDBusArgument &argument, const IntString &intString);
const QDBusArgument &operator>>(const QDBusArgument &argument, IntString &intString);

void registerIntStringMetaType();

#endif // !INTSTRING_H
