# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

# xdg-activation 支持（迁移自 dde-tray-loader 的 src/tray-wayland-integration）
# 依赖 Wayland private 头与 xdg-activation-v1 协议生成代码；供 dock 插件链接使用

find_package(PkgConfig REQUIRED)
find_package(${QT_NS} REQUIRED COMPONENTS Core Gui WaylandClient)
find_package(${DTK_NS} REQUIRED COMPONENTS Core)

pkg_check_modules(WaylandClient REQUIRED IMPORTED_TARGET wayland-client)
pkg_get_variable(WAYLAND_PROTOCOLS_DATADIR wayland-protocols pkgdatadir)

add_library(dock-xdgactivation STATIC
    xdgactivation.cpp
    xdgactivation.h
    xdgactivation_p.h
)
set_target_properties(dock-xdgactivation PROPERTIES POSITION_INDEPENDENT_CODE ON)

qt_generate_wayland_protocol_client_sources(dock-xdgactivation
    FILES
        ${WAYLAND_PROTOCOLS_DATADIR}/staging/xdg-activation/xdg-activation-v1.xml
)

target_include_directories(dock-xdgactivation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(dock-xdgactivation PUBLIC
    ${DTK_NS}::Core
    ${QT_NS}::Gui
    ${QT_NS}::WaylandClient
    PkgConfig::WaylandClient
)
target_link_libraries(dock-xdgactivation PRIVATE
    ${QT_NS}::WaylandClientPrivate
)
