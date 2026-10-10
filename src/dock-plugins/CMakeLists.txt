# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

# 托盘/dock 插件（PluginsItemInterfaceV2），迁移自 dde-tray-loader
# 产物安装到 /usr/lib/dde-dock/plugins/，由 dde-shell 的 trayplugin-loader 加载

add_subdirectory(wayland)
add_subdirectory(bluetooth)
add_subdirectory(sound)
add_subdirectory(brightness)
add_subdirectory(eye-comfort-mode)
add_subdirectory(wirelesscasting)

# dcc-dock-plugins 合并翻译域（bluetooth/sound/brightness/eye-comfort-mode）
file(GLOB DCC_DOCK_PLUGINS_TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/dcc-dock-plugins_*.ts)
file(GLOB_RECURSE DCC_DOCK_PLUGINS_SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sound/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sound/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/brightness/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/brightness/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/eye-comfort-mode/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/eye-comfort-mode/*.h)

add_custom_target(dcc-dock-plugins_translation ALL SOURCES ${DCC_DOCK_PLUGINS_TS_FILES})
qt_add_translations(dcc-dock-plugins_translation
    TS_FILES ${DCC_DOCK_PLUGINS_TS_FILES}
    SOURCES ${DCC_DOCK_PLUGINS_SRC_FILES}
    LUPDATE_OPTIONS -no-obsolete -no-ui-lines -locations none
    QM_FILES_OUTPUT_VARIABLE DCC_DOCK_PLUGINS_QM_FILES)
install(FILES ${DCC_DOCK_PLUGINS_QM_FILES}
    DESTINATION share/dcc-dock-plugins/translations)
