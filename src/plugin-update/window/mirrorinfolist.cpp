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

#include "mirrorinfolist.h"


MirrorInfo::MirrorInfo()
{

}

const QDBusArgument &operator>>(const QDBusArgument &argument, MirrorInfo &info)
{
    argument.beginStructure();
    argument >> info.m_id;
    argument >> info.m_url;
    argument >> info.m_name;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const MirrorInfo &info)
{
    argument.beginStructure();
    argument << info.m_id;
    argument << info.m_url;
    argument << info.m_name;
    argument.endStructure();

    return argument;
}

QDebug operator<<(QDebug argument, const MirrorInfo &info)
{
    argument << "mirror id: " << info.m_id;
    argument << "mirror url: " << info.m_url;
    argument << "mirror name: " << info.m_name;

    return argument;
}

void registerMirrorInfoListMetaType()
{
    qRegisterMetaType<MirrorInfo>();
    qDBusRegisterMetaType<MirrorInfo>();
    qRegisterMetaType<MirrorInfoList>();
    qDBusRegisterMetaType<MirrorInfoList>();
}
