cmake_minimum_required(VERSION 3.7)

set(PLUGIN_NAME "shell-network-plugin")

project(${PLUGIN_NAME})

# 启用 qt moc 的支持
set(CMAKE_AUTOMOC ON)
# 启用 qrc 资源文件的支持
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE SRCS "*.h" "*.cpp"  "../src/*.h" "../src/*.cpp" "../common-plugin/*.h" "../common-plugin/*.cpp" "../dde-network-dialog/dockpopupwindow.h" "../dde-network-dialog/dockpopupwindow.cpp")

find_package(Qt5 COMPONENTS Core Widgets DBus Network LinguistTools REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(DtkWidget REQUIRED)
#find_package(DtkCMake REQUIRED)
find_package(DdeDock REQUIRED)
find_package(KF5NetworkManagerQt REQUIRED)
#find_package(x11extras REQUIRED)


pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)
pkg_check_modules(QGSettings REQUIRED gsettings-qt)
pkg_check_modules(DDE-Dock REQUIRED dde-dock)

file(GLOB TS_FILES "translations/*.ts")
qt5_add_translation(QM_FILES ${TS_FILES})
add_custom_target(${TARGET_NAME}_language ALL DEPENDS ${QM_FILES})

add_definitions("${QT_DEFINITIONS} -DQT_PLUGIN")
add_library(${PLUGIN_NAME} SHARED ${SRCS} ../common-plugin/network.qrc)

set(NETWORKINTERFACE "../src")
list(REMOVE_ITEM NETWORKINTERFACE "utils.h")

get_target_property(KF5_QT_INCLUDE_DIRS KF5::NetworkManagerQt INTERFACE_INCLUDE_DIRECTORIES)

set(TRY_KF5_LIBRARIES
IMPORTED_LOCATION_DEBIAN
IMPORTED_LOCATION_NOCONFIG
)

if (NOT KF5_QT_LIBRARIES)
    find_library(KF5_QT_LIBRARIES libKF5NetworkManagerQt.so)
endif()

target_include_directories(${PLUGIN_NAME} PUBLIC
    ${NETWORKINTERFACE}
    ${DtkWidget_INCLUDE_DIRS}
    ${Qt5DBus_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
    ${QGSettings_INCLUDE_DIRS}
    #${DDE-Network-Core_INCLUDE_DIRS}
    ${DDE_DOCK_INCLUDE_DIR}
    ${Qt5Network_INCLUDE_DIRS}
    ${KF5_QT_INCLUDE_DIRS}
    "../common-plugin/"
    "interface"
)

target_link_libraries(${PLUGIN_NAME} PRIVATE
    ${DtkWidget_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Svg_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${QGSettings_LIBRARIES}
    #${DDE-Network-Core_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${KF5_QT_LIBRARIES}
    ${Qt5Network_LIBRARIES}
)

install(TARGETS ${PLUGIN_NAME} LIBRARY DESTINATION /lib/dde-session-shell/modules)
# 安装 .qm 文件
install(FILES ${QM_FILES} DESTINATION /usr/share/${PLUGIN_NAME}/translations)
install(FILES 10-network-manager.pkla DESTINATION /var/lib/polkit-l/localauthority/10-vendor.d)
