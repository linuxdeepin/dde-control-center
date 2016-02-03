/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "mirrorinfo.h"


MirrorInfo::MirrorInfo()
{

}

void MirrorInfo::registerMetaType()
{
    qRegisterMetaType<MirrorInfo>();
    qDBusRegisterMetaType<MirrorInfo>();
    qRegisterMetaType<MirrorInfoList>();
    qDBusRegisterMetaType<MirrorInfoList>();
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
