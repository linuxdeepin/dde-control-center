cmake_minimum_required(VERSION 3.18)

#--------------------------frame--------------------------
set(DCC_Library dde-control-center_frame)
set(DCCFrame_Name ${DCC_Library})
file(GLOB DCCFrame_SRCS
    "${DCC_PROJECT_ROOT_DIR}/include/*.h"
    "frame/*.cpp"
)

add_library(${DCCFrame_Name} SHARED
    ${DCCFrame_SRCS}
)

set(DCCFrame_Libraries
    ${DTK_NS}::Gui
    Qt::Gui
    Qt::Quick
)

target_link_libraries(${DCCFrame_Name} PRIVATE
    ${DCCFrame_Libraries}
)

set_target_properties(${DCCFrame_Name} PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION}
    SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR}
    OUTPUT_NAME dde-control-center
    EXPORT_NAME Control-Center
)

install(TARGETS ${DCCFrame_Name}
        EXPORT DdeControlCenterTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dde-control-center
)

install(EXPORT DdeControlCenterTargets
        FILE DdeControlCenterTargets.cmake
        NAMESPACE Dde::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter
)
#--------------------------qml-plugin--------------------------
add_subdirectory(frame/plugin)
#--------------------------dde-control-center--------------------------
set(Control_Center_Name dde-control-center)
file(GLOB Control_Center_SRCS
    "*.h"
    "*.cpp"
)

add_executable(${Control_Center_Name}
    ${Control_Center_SRCS}
    qrc/dcc.qrc
)

target_compile_definitions(${Control_Center_Name} PRIVATE CVERSION="${CMAKE_PROJECT_VERSION}")

target_include_directories(${Control_Center_Name} PRIVATE
    include/interface
    frame
)

set(Control_Center_Libraries
    ${DCC_Library}
    ${DTK_NS}::Gui
    Qt::Gui
    Qt::DBus
    Qt::Concurrent
    Qt::Quick
)

target_link_libraries(${Control_Center_Name} PRIVATE
    ${Control_Center_Libraries}
)

file(GLOB_RECURSE qmlFiles  "${DCC_PROJECT_ROOT_DIR}/qml/*.*")
add_custom_target(${Control_Center_Name}_qml ALL
    SOURCES ${qmlFiles}
)
add_custom_command(TARGET ${Control_Center_Name}_qml
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${DCC_PROJECT_ROOT_DIR}/qml" ${DCC_LIBDIR}
)

dcc_handle_plugin_translation(NAME ${Control_Center_Name} SOURCE_DIR ${DCC_PROJECT_ROOT_DIR})
# bin
if (BUILD_DCC_OLD)
    install(TARGETS ${Control_Center_Name} DESTINATION ${DCC_DEBUG_LIBDIR})
else()
    install(TARGETS ${Control_Center_Name} DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
install(DIRECTORY "${DCC_PROJECT_ROOT_DIR}/qml/" DESTINATION ${DCC_INSTALL_DIR})
#----------------------------install config------------------------------
#desktop
install(FILES ${DCC_PROJECT_ROOT_DIR}/misc/org.deepin.dde.control-center.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)

#service
configure_file(
    ${DCC_PROJECT_ROOT_DIR}/misc/org.deepin.dde.ControlCenter1.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/org.deepin.dde.ControlCenter1.service
    @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.deepin.dde.ControlCenter1.service
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/dbus-1/services)

# systemd service
if (NOT DEFINED SYSTEMD_USER_UNIT_DIR)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(Systemd REQUIRED systemd)
    pkg_get_variable(SYSTEMD_USER_UNIT_DIR systemd systemduserunitdir)
endif()

configure_file(
    ${CMAKE_SOURCE_DIR}/misc/systemd/dde-control-center.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/org.deepin.dde.control-center.service
    @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.deepin.dde.control-center.service
        DESTINATION ${SYSTEMD_USER_UNIT_DIR})

# dev files
file(GLOB HEADERS "${DCC_PROJECT_ROOT_DIR}/include/*")
set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/dde-control-center)
install(FILES ${HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR})
if (BUILD_DCC_OLD)
    configure_package_config_file(${DCC_PROJECT_ROOT_DIR}/misc/DdeControlCenterConfigOld.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter
        PATH_VARS INCLUDE_INSTALL_DIR
        INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
else()
    configure_package_config_file(${DCC_PROJECT_ROOT_DIR}/misc/DdeControlCenterConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter
        PATH_VARS INCLUDE_INSTALL_DIR
        INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
endif()

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter)

install(FILES "${DCC_PROJECT_ROOT_DIR}/misc/DdeControlCenterPluginMacros.cmake" DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter)

install(FILES ${DCC_PROJECT_ROOT_DIR}/misc/org.deepin.dde.controlcenter.metainfo.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)

#dde-grand-search-daemon conf
install(FILES ${DCC_PROJECT_ROOT_DIR}/misc/org.deepin.dde-grand-search.dde-control-center-setting.conf
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/dde-grand-search-daemon/plugins/searcher)

