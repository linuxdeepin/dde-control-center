/*
* Copyright (C) 2021 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     Zhang Qipeng <zhangqipeng@uniontech.com>
*
* Maintainer: Zhang Qipeng <zhangqipeng@uniontech.com>
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

#ifndef LOGINMODULEINTERFACE_H
#define LOGINMODULEINTERFACE_H

#include "base_module_interface.h"

namespace dss {
namespace module {

using Fun = void (*)(char *);

class LoginModuleInterface : public BaseModuleInterface
{
public:
    /**
     * @brief 模块的类型
     * @return ModuleType
     */
    ModuleType type() const override { return LoginType; }

    /**
     * @brief 在认证完成后，需要调用回调函数 void Fun(char *);
     * char *: json 格式的字符串，包含两个字段
     * string: 用户在系统的唯一标识（用户名或 uid）
     * bool: 认证结果（true: 成功，false: 失败）
     * Fun 在模块初始化时建议赋值 nullptr，在使用之前判断非 nullptr
     */
    virtual void setAuthFinishedCallBack(Fun) = 0;
};

} // namespace module
} // namespace dss

Q_DECLARE_INTERFACE(dss::module::LoginModuleInterface, "com.deepin.dde.shell.Modules.Login")

#endif // LOGINMODULEINTERFACE_H
