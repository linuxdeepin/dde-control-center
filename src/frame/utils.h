/*
* Copyright (C) 2019 ~ 2022 Deepin Technology Co., Ltd.
*
* Author:     guoyao <guoyao@uniontech.com>

* Maintainer: guoyao <guoyao@uniontech.com>

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include "interface/namespace.h"

#include <QObject>
#include <QDir>

#define IS_SERVER_SYSTEM (DSysInfo::UosServer == DSysInfo::uosType())                    // 是否是服务器版
#define IS_COMMUNITY_SYSTEM (DSysInfo::UosCommunity == DSysInfo::uosEditionType())       // 是否是社区版
#define IS_PROFESSIONAL_SYSTEM (DSysInfo::UosProfessional == DSysInfo::uosEditionType()) // 是否是专业版
#define IS_HOME_SYSTEM (DSysInfo::UosHome == DSysInfo::uosEditionType())                 // 是否是个人版
#define IS_EDUCATION_SYSTEM (DSysInfo::UosEducation == DSysInfo::uosEditionType())       // 是否是教育版
#define IS_DEEPIN_DESKTOP (DSysInfo::DeepinDesktop == DSysInfo::deepinType())            // 是否是Deepin桌面

namespace DCC_NAMESPACE {
class ModuleObject;

static const QString &CollapseConfgPath = QDir::tempPath() + "/dde-control-center-collapse.conf";
QByteArray getFileMd5(const QString &filePath);
ModuleObject *GetModuleByUrl(ModuleObject *const root, const QString &url);
QString GetUrlByModule(ModuleObject *const module);

}
