/*
 * Copyright (C) 2015 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef MIRRORINFO_H
#define MIRRORINFO_H

#include <QObject>
#include <QDBusArgument>
#include <QDebug>
#include <QtDBus>

class MirrorInfo
{
public:
    MirrorInfo();

    friend QDebug operator<<(QDebug argument, const MirrorInfo &info);
    friend QDBusArgument &operator<<(QDBusArgument &argument, const MirrorInfo &info);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, MirrorInfo &info);

public:
    QString m_id;
    QString m_name;
    QString m_url;
};

typedef QList<MirrorInfo> MirrorInfoList;
Q_DECLARE_METATYPE(MirrorInfo)
Q_DECLARE_METATYPE(MirrorInfoList)

void registerMirrorInfoListMetaType();

#endif // MIRRORINFO_H
