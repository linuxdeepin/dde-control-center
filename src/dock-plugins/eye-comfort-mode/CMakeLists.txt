set(PLUGIN_NAME eye-comfort-mode)

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
find_package(DDEShell REQUIRED)

# DtkDBusMacros 依赖 QT_VERSION_MAJOR 判断 Qt5/Qt6，此处固定为 Qt6
set(QT_VERSION_MAJOR 6)

pkg_check_modules(DDE_DOCK REQUIRED dde-dock)
pkg_check_modules(XCURSOR REQUIRED xcursor)

dtk_add_dbus_interface(DBUS_INTERFACES
    ${CMAKE_CURRENT_SOURCE_DIR}/../dbus/xml/org.deepin.dde.Appearance1.xml
    appearance1interface
)

add_definitions("${QT_DEFINITIONS} -DQT_PLUGIN")
add_library(${PLUGIN_NAME} SHARED ${DBUS_INTERFACES} ${SRCS} resources/eye-comfort-mode.qrc)

target_include_directories(${PLUGIN_NAME} PUBLIC
    ${DDE_DOCK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/util
)
target_link_libraries(${PLUGIN_NAME} PRIVATE
    ${DTK_NS}::Widget
    ${QT_NS}::Widgets
    ${QT_NS}::Svg
    ${QT_NS}::DBus
    ${XCURSOR_LIBRARIES}
    Dde::Shell
)
install(TARGETS ${PLUGIN_NAME} LIBRARY DESTINATION lib/dde-dock/plugins)
install(FILES "resources/dcc-eye-comfort-mode.dci" DESTINATION share/dde-dock/icons/dcc-setting)

file(GLOB EYE_COMFORT_TS_FILES "translations/*.ts")
qt_add_translation(EYE_COMFORT_QM_FILES ${EYE_COMFORT_TS_FILES})
add_custom_target(${PLUGIN_NAME}_translation ALL DEPENDS ${EYE_COMFORT_QM_FILES})
install(FILES ${EYE_COMFORT_QM_FILES} DESTINATION share/dock-eye-comfort-mode-plugin/translations)
