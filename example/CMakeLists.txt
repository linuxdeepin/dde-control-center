cmake_minimum_required(VERSION 3.7)

project(example)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_PREFIX_PATH $ENV{Qt5_DIR})
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(DdeControlCenter REQUIRED)

aux_source_directory(../dcc-network-plugin DCCPLUGINDIR)
aux_source_directory(../dcc-network-plugin/window DCCPLUGINWINDOWDIR)
aux_source_directory(../dcc-network-plugin/sections DCCPLUGINSECTIONS)
aux_source_directory(../dcc-network-plugin/sections/vpn DCCPLUGINSECTIONSVPN)
aux_source_directory(../dcc-network-plugin/settings DCCPLUGINSETTINGS)
aux_source_directory(../dcc-network-plugin/settings/vpn DCCPLUGINVPNSETTINGS)
aux_source_directory(../dcc-network-plugin/themes DCCPLUGINTHEMES)
aux_source_directory(../src NETWORKDIR)
aux_source_directory(../src/realize REALIZEDIR)

file(GLOB_RECURSE QRCFILE "./example.qrc"
    "../dcc-network-plugin/icons/dcc-network-plugin.qrc"
    "../dcc-network-plugin/icons/actions/*.svg"
    "../dcc-network-plugin/icons/icons/*.svg")

set(COMPILEFILES
    ./networkhandler.h
    ./networkhandler.cpp
    ./main.cpp
    ./dccplugintestwidget.h
    ./dccplugintestwidget.cpp)

#list(REMOVE_ITEM NETWORKDIR "../dde-network-core/debian")

add_executable(${PROJECT_NAME}
    ${COMPILEFILES}
    ${DCCPLUGINDIR}
    ${DCCPLUGINWINDOWDIR}
    ${NETWORKDIR}
    ${DCCPLUGINSECTIONS}
    ${DCCPLUGINSECTIONSVPN}
    ${DCCPLUGINSETTINGS}
    ${DCCPLUGINVPNSETTINGS}
    ${DCCPLUGINTHEMES}
    ${REALIZEDIR}
    ${QRCFILE})

find_package(Qt5 COMPONENTS Core DBus Network REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(KF5NetworkManagerQt REQUIRED)
find_package(DtkWidget REQUIRED)

get_target_property(KF5_QT_INCLUDE_DIRS KF5::NetworkManagerQt INTERFACE_INCLUDE_DIRECTORIES)

foreach(LETTER ${TRY_KF5_LIBRARIES})
    if (NOT KF5_QT_LIBRARIES)
        get_target_property(KF5_QT_LIBRARIES KF5::NetworkManagerQt ${LETTER})
    endif()
endforeach()

if (NOT KF5_QT_LIBRARIES)
    find_library(KF5_QT_LIBRARIES libKF5NetworkManagerQt.so)
endif()

set(TRY_KF5_LIBRARIES
IMPORTED_LOCATION_DEBIAN
IMPORTED_LOCATION_NOCONFIG
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ../src
    ../src/realize
    ../dcc-network-plugin
    ${DdeControlCenter_INCLUDE_DIR}
    ${DtkWidget_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5DBus_INCLUDE_DIRS}
    ${Qt5Widget_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
    ${QGSettings_INCLUDE_DIRS}
    ${LibNM_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
    ${KF5_QT_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${DdeControlCenter_LIBRARIES}
    ${DtkWidget_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${LibNM_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Network_LIBRARIES}
    ${QGSettings_LIBRARIES}
    ${KF5_QT_LIBRARIES})

add_custom_command(
    OUTPUT path.pb.cc path.pb.h
    COMMAND protoc
    ARGS --cpp_out . ${CMAKE_CURRENT_SOURCE_DIR}/path.proto -I ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS path.proto)
