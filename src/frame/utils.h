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
#include <QStandardPaths>

DCC_BEGIN_NAMESPACE
class ModuleObject;

__attribute__((unused)) static bool IsModulesLoaded = false;

static const QString &CollapseConfgPath = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first() + "/dde-control-center-collapse.conf";
QByteArray getFileMd5(const QString &filePath);
ModuleObject *GetModuleByUrl(ModuleObject *const root, const QString &url);
QString GetUrlByModule(ModuleObject *const module);

DCC_END_NAMESPACE
