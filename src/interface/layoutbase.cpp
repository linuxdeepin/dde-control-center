/*
* Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#include "interface/layoutbase.h"
DCC_USE_NAMESPACE
// 0xA0000000 = 0x80000000|0x20000000 0x80000000为用户设置位 0x20000000为配置设置位
// 0x50000000 = 0x40000000|0x10000000 0x4000000为用户设置位 0x10000000为配置设置位
#define DCC_ALL_HIDDEN 0xA0000000
#define DCC_ALL_DISABLED 0x50000000

ModuleObject *LayoutBase::autoExpand(ModuleObject *const module, ModuleObject *const child)
{
    return child ? child : module->children(0);
}

bool LayoutBase::IsHiden(ModuleObject *const module)
{
    return module ? module->getFlagState(DCC_ALL_HIDDEN) : true;
}

bool LayoutBase::IsHidenFlag(uint32_t flag)
{
    return DCC_ALL_HIDDEN & flag;
}

bool LayoutBase::IsDisabled(ModuleObject *const module)
{
    return module ? module->getFlagState(DCC_ALL_DISABLED) : true;
}

bool LayoutBase::IsDisabledFlag(uint32_t flag)
{
    return DCC_ALL_DISABLED & flag;
}
