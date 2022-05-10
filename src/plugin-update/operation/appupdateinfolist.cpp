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

#include "appupdateinfolist.h"

AppUpdateInfo::AppUpdateInfo()
{

}

const QDBusArgument &operator>>(const QDBusArgument &argument, AppUpdateInfo &info)
{
    argument.beginStructure();
    argument >> info.m_packageId;
    argument >> info.m_name;
    argument >> info.m_icon;
    argument >> info.m_currentVersion;
    argument >> info.m_avilableVersion;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const AppUpdateInfo &info)
{
    argument.beginStructure();
    argument << info.m_packageId;
    argument << info.m_name;
    argument << info.m_icon;
    argument << info.m_currentVersion;
    argument << info.m_avilableVersion;
    argument.endStructure();

    return argument;
}

QDebug operator<<(QDebug argument, const AppUpdateInfo &info)
{
    argument << "packageId: " << info.m_packageId;
    argument << "name: " << info.m_name;
    argument << "icon: " << info.m_icon;
    argument << "currentVer: " << info.m_currentVersion;
    argument << "avilableVer: " << info.m_avilableVersion;

    return argument;
}

void registerAppUpdateInfoListMetaType()
{
    qRegisterMetaType<AppUpdateInfo>();
    qDBusRegisterMetaType<AppUpdateInfo>();
    qRegisterMetaType<AppUpdateInfoList>();
    qDBusRegisterMetaType<AppUpdateInfoList>();
}
