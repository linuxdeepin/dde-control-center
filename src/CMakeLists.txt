cmake_minimum_required(VERSION 3.7)

project(dde-network-core)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH $ENV{Qt5_DIR})

file(GLOB_RECURSE SOURCEFILES "*.h" "*.cpp")
add_library(${PROJECT_NAME} SHARED ${SOURCEFILES})

find_package(Qt5 COMPONENTS Core DBus Network REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(KF5NetworkManagerQt REQUIRED)

pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)
pkg_check_modules(QGSettings REQUIRED gsettings-qt)

get_target_property(KF5_QT_INCLUDE_DIRS KF5::NetworkManagerQt INTERFACE_INCLUDE_DIRECTORIES)

set(TRY_KF5_LIBRARIES
IMPORTED_LOCATION_DEBIAN
IMPORTED_LOCATION_NOCONFIG
)

if (NOT KF5_QT_LIBRARIES)
    find_library(KF5_QT_LIBRARIES libKF5NetworkManagerQt.so)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    realize
    ${Qt5DBus_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
    ${QGSettings_INCLUDE_DIRS}
    ${LibNM_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
    ${KF5_QT_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${Qt5Core_LIBRARIES}
    ${LibNM_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Network_LIBRARIES}
    ${QGSettings_LIBRARIES}
    ${KF5_QT_LIBRARIES})

configure_file(dde-network-core.pc.in ../dde-network-core.pc @ONLY)

set(INTERFACEFILES
    dslcontroller.h
    hotspotcontroller.h
    networkconst.h
    networkcontroller.h
    networkdetails.h
    networkdevicebase.h
    proxycontroller.h
    vpncontroller.h
    wireddevice.h
    wirelessdevice.h)

set(CMAKE_INSTALL_PREFIX "/usr")

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets LIBRARY DESTINATION lib)
install(FILES ${CMAKE_BINARY_DIR}/dde-network-core.pc DESTINATION lib/pkgconfig)
install(FILES ${INTERFACEFILES} DESTINATION include/libddenetworkcore)
