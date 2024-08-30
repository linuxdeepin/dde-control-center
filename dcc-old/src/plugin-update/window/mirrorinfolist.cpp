//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
