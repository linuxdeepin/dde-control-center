/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     justforlxz <justforlxz@outlook.com>
 *
 * Maintainer: justforlxz <justforlxz@outlook.com>
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

#pragma once

#include <DSysInfo>
#include <QMetaType>

DCORE_USE_NAMESPACE

const DSysInfo::UosType UosType = DSysInfo::uosType();
const DSysInfo::UosEdition UosEdition = DSysInfo::uosEditionType();
const bool IsServerSystem = (DSysInfo::UosServer == UosType);//是否是服务器版
const bool IsCommunitySystem = (DSysInfo::UosCommunity == UosEdition);//是否是社区版
const bool IsProfessionalSystem = (DSysInfo::UosProfessional == UosEdition);//是否是专业版
const bool IsHomeSystem = (DSysInfo::UosHome == UosEdition);//是否是个人版
const bool IsDeepinDesktop = (DSysInfo::DeepinDesktop == DSysInfo::deepinType());//是否是Deepin桌面

enum SyncType : int {
    Network,
    Sound,
    Mouse,
    Update,
    Dock,
    Launcher,
    Wallpaper,
    Theme,
    Power,
    Corner
};

enum SyncState : int {
    Succeed,
    Syncing,
    Failed,
};
