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

#ifndef MODULE_INTERFACE_H
#define MODULE_INTERFACE_H

#include <QtCore>

namespace dss {
namespace module {

class BaseModuleInterface
{
public:
    /**
     * @brief The ModuleType enum
     * 模块的类型
     */
    enum ModuleType {
        LoginType,
        TrayType
    };

    virtual ~BaseModuleInterface() = default;

    /**
     * @brief 界面相关的初始化
     * 插件在非主线程加载，故界面相关的初始化需要放在这个方法里，由主程序调用并初始化
     */
    virtual void init() = 0;

    /**
     * @brief 键值，用于与其它模块区分
     * @return QString
     */
    virtual QString key() const = 0;

    /**
     * @brief 模块想显示的界面
     * content 对象由模块自己管理
     * @return QWidget*
     */
    virtual QWidget *content() = 0;

    /**
     * @brief 模块的类型
     * @return ModuleType
     */
    virtual ModuleType type() const = 0;
};

} // namespace module
} // namespace dss

Q_DECLARE_INTERFACE(dss::module::BaseModuleInterface, "com.deepin.dde.shell.Modules")

#endif // MODULE_INTERFACE_H
