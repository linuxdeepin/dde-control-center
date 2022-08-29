cmake_minimum_required(VERSION 3.7)

project(dde-network-core)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(GNUInstallDirs)
set(CMAKE_PREFIX_PATH $ENV{Qt5_DIR})

file(GLOB_RECURSE SOURCEFILES "*.h" "*.cpp")
add_library(${PROJECT_NAME} SHARED ${SOURCEFILES})

find_package(Qt5 COMPONENTS Core DBus Network REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(KF5NetworkManagerQt REQUIRED)

pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)
pkg_check_modules(QGSettings REQUIRED gsettings-qt)
pkg_check_modules(LibNM REQUIRED IMPORTED_TARGET libnm)

# KF5 network manager
 get_target_property(KF5_QT_INCLUDE_DIRS KF5::NetworkManagerQt INTERFACE_INCLUDE_DIRECTORIES)

 set(TRY_KF5_LIBRARIES
 IMPORTED_LOCATION_DEBIAN
 IMPORTED_LOCATION_NOCONFIG
 )

 foreach(LETTER ${TRY_KF5_LIBRARIES})
     if (NOT KF5_QT_LIBRARIES)
         get_target_property(KF5_QT_LIBRARIES KF5::NetworkManagerQt ${LETTER})
     endif()
 endforeach()

 if (NOT KF5_QT_LIBRARIES)
     find_library(KF5_QT_LIBRARIES libKF5NetworkManagerQt.so)
 endif()
 # end KF5 network manager

target_include_directories(${PROJECT_NAME} PUBLIC
    realize
    ${Qt5DBus_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
    ${QGSettings_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
    ${KF5_QT_INCLUDE_DIRS}
    PkgConfig::LibNM
)

message(STATUS ${LibNM_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${Qt5Core_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Network_LIBRARIES}
    ${QGSettings_LIBRARIES}
    ${KF5_QT_LIBRARIES}
    PkgConfig::LibNM
)

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

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${CMAKE_BINARY_DIR}/dde-network-core.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
install(FILES ${INTERFACEFILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libddenetworkcore)
install(FILES ../config/org.deepin.dde.network.json DESTINATION ${CMAKE_INSTALL_DATADIR}/dsg/configs/org.deepin.dde.network)
