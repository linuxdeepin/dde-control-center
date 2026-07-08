// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import org.deepin.dcc 1.0

/**
 * @brief 磁盘管理插件 —— 元数据入口（侧边栏菜单项）
 *
 * 【插件加载流程 - Module 阶段】
 * DccPluginLoader 在加载插件时，首先查找 {name}.qml (即 DiskManager.qml)，
 * 作为插件的"元数据入口"。
 *
 * 该文件的作用：
 *   1. 定义根 DccObject，注册到全局对象树
 *   2. 提供侧边栏的显示名称、图标、排序权重
 *   3. 控制模块可见性
 *
 * 文件命名规则：
 *   必须与插件名同名（大小写敏感），框架通过 {name}.qml 查找。
 *   插件名由 CMakeLists.txt 中的 PLUGIN_NAME 定义。
 *
 * 关键属性说明：
 *   - name:      全局唯一的标识符，用于 URL 导航和 parentName 引用
 *   - parentName: 父节点路径，"root" 表示顶级菜单
 *   - displayName: 侧边栏显示的文本，使用 qsTr() 支持翻译
 *   - icon:       侧边栏图标（DCI 图标名称或主题图标名）
 *   - weight:     排序权重（越大越靠后），建议与同类插件保持间隔 10
 *   - page:       指向主页面 QML 组件
 *
 * 【关于 page 属性】
 *   这里不设置 page，因为 DiskManagerMain.qml 会自动加载。
 *   page 是当用户点击侧边栏菜单项时渲染的组件。
 *   DiskManagerMain.qml 中的 DccObject 会通过 parentName: "diskmanager"
 *   自动挂载到当前 DccObject 下成为子节点。
 */
DccObject {
    id: root
    name: "diskmanager"
    parentName: "root"
    displayName: qsTr("Disk Manager")
    icon: "drive-harddisk"
    weight: 350

    // 模块可见性可通过 DConfig 控制
    // 在 org.deepin.dde.control-center.json 的 hideModule/disableModule 中配置
}