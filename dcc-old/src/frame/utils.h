//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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

inline const QString PLUGIN_DIRECTORY = QStringLiteral(DCC_DefaultModuleDirectory);
inline const QString OLD_PLUGIN_DIRECTORY = QStringLiteral("/usr/lib/dde-control-center/modules/");
}
