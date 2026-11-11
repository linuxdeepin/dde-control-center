set(PLUGIN_NAME dock-sound)

project(${PLUGIN_NAME})

file(GLOB_RECURSE SRCS
    "*.h"
    "*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../common/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../common/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../common/util/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../common/util/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dbus/types/audioport.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dbus/types/audioport.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dbus/types/audioportlist.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dbus/types/audioportlist.cpp"
)

find_package(PkgConfig REQUIRED)
find_package(${QT_NS} REQUIRED COMPONENTS Core Gui Widgets DBus Svg)
find_package(${DTK_NS} REQUIRED COMPONENTS Core Gui Widget Tools)
find_package(DDEShell REQUIRED)

# DtkDBusMacros 依赖 QT_VERSION_MAJOR 判断 Qt5/Qt6，此处固定为 Qt6
set(QT_VERSION_MAJOR 6)

pkg_check_modules(DDE_DOCK REQUIRED dde-dock)
pkg_check_modules(XCURSOR REQUIRED xcursor)

dtk_add_dbus_interface(DBUS_INTERFACES
    ${CMAKE_CURRENT_SOURCE_DIR}/../dbus/xml/org.deepin.dde.Audio1.xml
    audio1interface
)

dtk_add_dbus_interface(DBUS_INTERFACES
    ${CMAKE_CURRENT_SOURCE_DIR}/../dbus/xml/org.deepin.dde.Audio1.Sink.xml
    audio1sinkinterface
)

# 目标名 dock-sound 避免与 dcc-plugin-sound 的 sound 模块冲突，产物名仍为 libsound.so
add_definitions("${QT_DEFINITIONS} -DQT_PLUGIN")
add_library(${PLUGIN_NAME} SHARED ${DBUS_INTERFACES} ${SRCS} resources/sound.qrc)
set_target_properties(${PLUGIN_NAME} PROPERTIES OUTPUT_NAME sound)

target_include_directories(${PLUGIN_NAME} PUBLIC
    ${DDE_DOCK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../dbus
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/util
)
target_link_libraries(${PLUGIN_NAME} PRIVATE
    ${XCURSOR_LIBRARIES}
    ${DTK_NS}::Widget
    ${QT_NS}::DBus
    ${QT_NS}::Widgets
    ${QT_NS}::Svg
    Dde::Shell
)
install(TARGETS ${PLUGIN_NAME} LIBRARY DESTINATION lib/dde-dock/plugins/system-trays)

dtk_add_config_meta_files(APPID org.deepin.dde.dock
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/../configs/org.deepin.dde.dock.plugin.sound.json
)
dtk_add_config_meta_files(APPID org.deepin.dde.tray-loader
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/../configs/org.deepin.dde.dock.plugin.sound.json
)

file(GLOB SOUND_TS_FILES "translations/*.ts")
qt_add_translation(SOUND_QM_FILES ${SOUND_TS_FILES})
add_custom_target(${PLUGIN_NAME}_translation ALL DEPENDS ${SOUND_QM_FILES})
install(FILES ${SOUND_QM_FILES} DESTINATION share/dock-sound-plugin/translations)
