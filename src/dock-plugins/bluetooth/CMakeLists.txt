# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

set(PLUGIN_NAME dock-bluetooth)

project(${PLUGIN_NAME})

file(GLOB_RECURSE SRCS
    "*.h"
    "*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../common/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../common/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../common/util/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../common/util/*.cpp"
)

find_package(PkgConfig REQUIRED)
find_package(${QT_NS} REQUIRED COMPONENTS Core Gui Widgets DBus Svg)
find_package(${DTK_NS} REQUIRED COMPONENTS Core Gui Widget Tools)

# DtkDBusMacros 依赖 QT_VERSION_MAJOR 判断 Qt5/Qt6，此处固定为 Qt6
set(QT_VERSION_MAJOR 6)

pkg_check_modules(DDE_DOCK REQUIRED dde-dock)
pkg_check_modules(XCURSOR REQUIRED xcursor)

dtk_add_dbus_interface(DBUS_INTERFACES
    ${CMAKE_CURRENT_SOURCE_DIR}/../dbus/xml/org.deepin.dde.Bluetooth1.xml
    bluetooth1interface
)

dtk_add_dbus_interface(DBUS_INTERFACES
    ${CMAKE_CURRENT_SOURCE_DIR}/../dbus/xml/org.deepin.dde.AirplaneMode1.xml
    airplanemode1interface
)

# 目标名 dock-bluetooth 避免与 dcc-plugin-bluetooth 的 blueTooth 模块冲突，产物名仍为 libbluetooth.so
add_definitions("${QT_DEFINITIONS} -DQT_PLUGIN")
add_library(${PLUGIN_NAME} SHARED ${DBUS_INTERFACES} ${SRCS} resources/bluetooth.qrc)
set_target_properties(${PLUGIN_NAME} PROPERTIES OUTPUT_NAME bluetooth)

target_include_directories(${PLUGIN_NAME} PUBLIC
    ${DDE_DOCK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/util
)
target_link_libraries(${PLUGIN_NAME} PRIVATE
    ${XCURSOR_LIBRARIES}
    ${DTK_NS}::Widget
    ${QT_NS}::Core
    ${QT_NS}::Gui
    ${QT_NS}::Widgets
    ${QT_NS}::Svg
    ${QT_NS}::DBus
    dock-xdgactivation
)
install(TARGETS ${PLUGIN_NAME} LIBRARY DESTINATION lib/dde-dock/plugins/system-trays)
install(FILES "resources/dcc-bluetooth.dci" DESTINATION share/dde-dock/icons/dcc-setting)

dtk_add_config_meta_files(APPID org.deepin.dde.dock
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/../configs/org.deepin.dde.dock.plugin.bluetooth.json
)
dtk_add_config_meta_files(APPID org.deepin.dde.tray-loader
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/../configs/org.deepin.dde.dock.plugin.bluetooth.json
)
