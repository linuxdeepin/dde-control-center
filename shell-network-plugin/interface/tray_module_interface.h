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

#ifndef TRAYMODULEINTERFACE_H
#define TRAYMODULEINTERFACE_H

#include "base_module_interface.h"

#include <QWidget>

namespace dss {
namespace module {

class TrayModuleInterface : public BaseModuleInterface
{
public:
    /**
     * @brief 模块的类型
     * @return ModuleType
     */
    ModuleType type() const override { return TrayType; }

    virtual QString icon() const = 0;

    /**
     * @brief itemTipsWidget 插件的图标界面，比icon接口的图标优先级更高
     * @param itemKey
     * @return
     */
    virtual QWidget *itemWidget() const = 0;

    /**
     * @brief itemTipsWidget 插件的tip界面，返回的指针由锁屏进行管理
     * @param itemKey
     * @return
     */
    virtual QWidget *itemTipsWidget() const = 0;

    /**
     * @brief itemContextMenu 插件的右键菜单，其格式可参考项目的示例network插件
     * @return
     */
    virtual const QString itemContextMenu() const = 0;

    /**
     * @brief invokedMenuItem 插件菜单点击后的响应实现，由插件自行实现，锁屏会在合适的时机进行调用
     * @param menuId 菜单的id
     * @param checked 菜单是否是勾选状态
     */
    virtual void invokedMenuItem(const QString &menuId, const bool checked) const = 0;

};

} // namespace module
} // namespace dss

Q_DECLARE_INTERFACE(dss::module::TrayModuleInterface, "com.deepin.dde.shell.Modules.Tray")

#endif // TRAYMODULEINTERFACE_H
