# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

set(PLUGIN_NAME dock-wirelesscasting-plugin)

project(${PLUGIN_NAME})

find_package(PkgConfig REQUIRED)
find_package(${QT_NS} REQUIRED COMPONENTS Core Gui Widgets DBus Svg)
find_package(${DTK_NS} REQUIRED COMPONENTS Core Gui Widget Tools)

# DtkDBusMacros 依赖 QT_VERSION_MAJOR 判断 Qt5/Qt6，此处固定为 Qt6
set(QT_VERSION_MAJOR 6)

pkg_check_modules(DDE_DOCK REQUIRED dde-dock)

dtk_add_dbus_interface(DBUS_INTERFACES
    ${CMAKE_CURRENT_SOURCE_DIR}/../dbus/xml/org.deepin.dde.Display1.xml
    display1interface
)

option(ENABLE_WIRELESS_CASTING "Enable wireless casting" OFF)
if(ENABLE_WIRELESS_CASTING)
    add_compile_definitions(WIRELESS_CASTING_ENABLED)
endif()

file(GLOB WIRELESSCASTING_SRCS
    "src/*.cpp"
    "src/widget/*.cpp"
    "common/*.cpp"
)

file(GLOB WIRELESSCASTING_TYPES_SRCS
    "../dbus/types/*.cpp"
)

add_definitions("${QT_DEFINITIONS} -DQT_PLUGIN")
add_library(${PLUGIN_NAME} SHARED
    ${DBUS_INTERFACES}
    ${WIRELESSCASTING_SRCS}
    ${WIRELESSCASTING_TYPES_SRCS}
    res/resources.qrc
    translations/translations.qrc
)

target_include_directories(${PLUGIN_NAME} PUBLIC
    ${DDE_DOCK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widget
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../dbus
)
target_link_libraries(${PLUGIN_NAME} PRIVATE
    ${DTK_NS}::Widget
    ${DTK_NS}::Core
    ${QT_NS}::Widgets
    dock-xdgactivation
)
install(TARGETS ${PLUGIN_NAME} LIBRARY DESTINATION lib/dde-dock/plugins)
install(FILES "res/dcc-wireless-casting.dci" DESTINATION share/dde-dock/icons/dcc-setting)

file(GLOB WIRELESSCASTING_TS_FILES "translations/*.ts")
qt_add_translation(WIRELESSCASTING_QM_FILES ${WIRELESSCASTING_TS_FILES})
add_custom_target(dock-wirelesscasting-plugin_translation ALL
    DEPENDS ${WIRELESSCASTING_QM_FILES}
)
install(FILES ${WIRELESSCASTING_QM_FILES} DESTINATION share/dock-wirelesscasting-plugin/translations)
