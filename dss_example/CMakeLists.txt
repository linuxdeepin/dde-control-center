cmake_minimum_required(VERSION 3.7)

project(dss_example)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_PREFIX_PATH $ENV{Qt5_DIR})
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(DdeControlCenter REQUIRED)
find_package(Qt5 COMPONENTS Core DBus Network REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(KF5NetworkManagerQt REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(DdeDock REQUIRED)
find_package(DdeSessionShell REQUIRED)

aux_source_directory(../dss-network-plugin DSSPLUGINTHEMES)
aux_source_directory(../dss-network-plugin/notification DSSPLUGINNOTIFICATION)
aux_source_directory(../common-plugin COMMONPLUGINS)
aux_source_directory(../common-plugin/widgets COMMONPLUGINWIDGET)
aux_source_directory(../common-plugin/item COMMONPLUGINITEM)
aux_source_directory(../src NETWORKDIR)
aux_source_directory(../src/realize REALIZEDIR)
aux_source_directory(. COMPILEFILES)

get_target_property(KF5_QT_INCLUDE_DIRS KF5::NetworkManagerQt INTERFACE_INCLUDE_DIRECTORIES)

#set(COMPILEFILES ./*.h ./*.cpp)

add_executable(${PROJECT_NAME}
    ${COMPILEFILES}
    ${NETWORKDIR}
    ${DSSPLUGINTHEMES}
    ${DSSPLUGINNOTIFICATION}
    ${COMMONPLUGINS}
    ${COMMONPLUGINWIDGET}
    ${COMMONPLUGINITEM}
    ${REALIZEDIR})

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
    ../dss-network-plugin
    ../common-plugin
    ../common-plugin/widgets
    ../common-plugin/item
    ../dss-network-plugin/interface
    ${DdeControlCenter_INCLUDE_DIR}
    ${DDE_DOCK_INCLUDE_DIR}
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
    ${DDE_DOCK_LIBRARIES}
    ${DtkWidget_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${LibNM_LIBRARIES}
    ${Qt5Svg_LIBRARIES}
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
