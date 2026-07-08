cmake_minimum_required(VERSION 3.18)

# ★ 插件名（只支持字母+数字）
# 该名称必须与 DiskManager.qml 中 DccObject 的 name 属性保持一致
# 不可包含 / 等特殊字符
set(PLUGIN_NAME "diskmanager")

find_package(Qt6 COMPONENTS Core LinguistTools REQUIRED)
find_package(DdeControlCenter REQUIRED)

# 收集 C++ 源文件
file(GLOB_RECURSE PLUGIN_SRCS
    "operation/*.cpp"
    "operation/*.h"
)

# ============================================================
# 构建 C++ 共享库 (.so)
# add_library(... MODULE) 告诉 CMake 这是一个 Qt 插件模块
# ============================================================
add_library(${PLUGIN_NAME} MODULE
    ${PLUGIN_SRCS}
)

target_link_libraries(${PLUGIN_NAME} PRIVATE
    Dde::Control-Center   # 链接 dde-control-center 框架库
    Qt6::Core
)

# ============================================================
# ★ 关键宏: 安装插件 ★
# dcc_install_plugin 由 DdeControlCenterPluginMacros.cmake 提供，
# 它会做以下事情:
#   1. 调用 dcc_build_plugin 注册 QML 模块
#   2. 将 QML 文件编译为 Qt 资源
#   3. 安装到 ${CMAKE_INSTALL_LIBDIR}/dde-control-center/plugins_v1.1/diskmanager/
#   4. 自动处理 AUTOMOC（使 DCC_FACTORY_CLASS 宏生效）
# ============================================================
dcc_install_plugin(NAME ${PLUGIN_NAME} TARGET ${PLUGIN_NAME})

# 处理翻译文件（自动扫描 qml/cpp 中的 qsTr() 并生成 .ts）
# 指定 SOURCE_DIR 限定扫描范围，防止 lupdate 扫描到其他插件目录

# ★ 注意 ★
# QML 文件的命名要求（由 dcc_build_plugin 强制检查）:
#   1. 首字母必须大写（如 DiskManager.qml, DiskManagerMain.qml）
#   2. 入口元数据文件: {PLUGIN_NAME}.qml（即 diskmanager.qml → 错误！必须 DiskManager.qml）
#   3. 主页面文件: {PLUGIN_NAME}Main.qml（即 DiskManagerMain.qml）
#   4. 不可使用 main.qml（已废弃）
