cmake_minimum_required(VERSION 3.18)

set(DVERSION "6.0.44" CACHE STRING "define project version")
set(BUILD_DOCS OFF CACHE BOOL "Generate doxygen-based documentation")

set(PROJECT_NAME dde-control-center)
project(${PROJECT_NAME}
    VERSION ${DVERSION}
    DESCRIPTION "Deepin Control Center"
    HOMEPAGE_URL "https://github.com/linuxdeepin/dde-control-center"
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(ENABLE_ASAN "Enable asan" OFF)
option(BUILD_TESTING "UNIT test" OFF)

set(QT_NS Qt6)
set(DTK_NS Dtk6)
set(ASQT_NS AppStreamQt)

# 设置静态库文件目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# 动态库文件目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# 可执行文件目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# INFO:
# plugins can be disabled and their options
# plugin-authentication : DISABLE_AUTHENTICATION
# plugin-update : DISABLE_UPDATE
# plugin-keyboard: DISABLE_LANGUAGE to disable language panel

option(DISABLE_AUTHENTICATION "disable build authentication plugins" OFF)
option(DISABLE_UPDATE "disable build update plugins" OFF)
option(DISABLE_LANGUAGE "disable lanugage settings in control center" OFF)
option(USE_DEEPIN_ZONE "enable special timezone file on deepin" OFF)
option(DISABLE_SOUND_ADVANCED "disable sound advanced settings" OFF)
set(DEEPIN_TIME_ZONE_PATH "/usr/share/dde/zoneinfo/zone1970.tab" CACHE STRING "deepin timezone path")
set(LOCALE_I18N_PATH "/usr/share/i18n/SUPPORTED" CACHE STRING "Supported locale path")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/misc;${CMAKE_MODULE_PATH};${ECM_MODULE_PATH};${PROJECT_SOURCE_DIR}/misc")
include(DdeControlCenterPluginMacros)

# asan 自己有内存泄露，暂不使用
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(UNITTEST ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror=return-type -fno-omit-frame-pointer -Wextra")
    if(ENABLE_ASAN)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
        add_definitions(-DUSE_ASAN)
    endif()
else()
    # generate qm
    execute_process(COMMAND bash "misc/translate_generation.sh" "${CMAKE_CURRENT_BINARY_DIR}"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    # generate desktop translate
    execute_process(COMMAND bash "misc/translate_ts2desktop.sh"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

if (BUILD_TESTING)
    set(UNITTEST ON)
endif()

set(BUILD_PLUGIN ON)

if (NOT BUILD_PLUGIN)
    set(UNITTEST OFF)
endif()
# GNU 默认

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(UT_COMPILER -fprofile-arcs -ftest-coverage)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(UT_COMPILER -fprofile-instr-generate -ftest-coverage)
endif()

if (USE_DEEPIN_ZONE)
    add_definitions(-DUSE_DEEPIN_ZONE)
    add_definitions(-DDEEPIN_TIME_ZONE_PATH="${DEEPIN_TIME_ZONE_PATH}")
endif ()

add_definitions(-DLOCALE_I18N_PATH="${LOCALE_I18N_PATH}")

# 增加安全编译参数
ADD_DEFINITIONS(${SAFE_COMPILER})

# Install settings
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif ()
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GoogleTest)

if(BUILD_DOCS)
    add_subdirectory(docs)
endif()

set(DCC_TRANSLATION_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/translations" CACHE STRING "Install dir for dde-control-center translate files")

add_definitions(-DTRANSLATE_READ_DIR="${CMAKE_INSTALL_PREFIX}/${DCC_TRANSLATION_INSTALL_DIR}")
# 输出目录
set(DCC_LIBDIR ${PROJECT_BINARY_DIR}/lib)
# 插件目录
set(DCC_PLUGINS_DIR plugins_v1.0)
# 安装目录
set(DCC_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/dde-control-center)
set(DCC_PREFIX "")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DCC_DEBUG_LIBDIR ${DCC_LIBDIR})
else()
    set(DCC_DEBUG_LIBDIR ${DCC_INSTALL_DIR})
    set(DCC_PREFIX "/usr/")
endif()

# 插件安装目录
set(DCC_PLUGINS_INSTALL_DIR ${DCC_DEBUG_LIBDIR}/${DCC_PLUGINS_DIR} CACHE STRING "Install dir for dde-control-center plugins")
# 插件读取目录
set(MODULE_READ_DIR "${DCC_PREFIX}${DCC_DEBUG_LIBDIR}" CACHE STRING "Dir to find dde-control-center modules")
set(PLUGINS_READ_DIR "${DCC_PREFIX}${DCC_PLUGINS_INSTALL_DIR}" CACHE STRING "Dir to find dde-control-center plugins")

add_definitions(-DDefaultModuleDirectory="${MODULE_READ_DIR}")
add_definitions(-DDefaultPluginsDirectory="${PLUGINS_READ_DIR}")

# DdeControlCenterPluginMacros.cmake 使用
set(DDE_CONTROL_CENTER_PLUGIN_DIR ${DCC_PLUGINS_DIR})
set(DDE_CONTROL_CENTER_TRANSLATION_INSTALL_DIR ${DCC_TRANSLATION_INSTALL_DIR})
set(DDE_CONTROL_CENTER_PLUGIN_INSTALL_DIR ${DCC_PLUGINS_INSTALL_DIR})

set(LOCALSTATE_READ_DIR "${CMAKE_INSTALL_FULL_LOCALSTATEDIR}" CACHE STRING "Dir to find modifiable single-machine data")
add_definitions(-DVARDIRECTORY="${LOCALSTATE_READ_DIR}")
# Find the library
find_package(PkgConfig REQUIRED)
find_package(${DTK_NS} REQUIRED COMPONENTS Core Gui  )
# find_package(QT NAMES ${QT_NS} REQUIRED COMPONENTS Core Quick)
find_package(${QT_NS} COMPONENTS Core Quick Gui Network DBus Concurrent Test LinguistTools REQUIRED)
# find_package(${QT_NS} COMPONENTS Widgets Network DBus Concurrent Multimedia Svg Test REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
# pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "sw_64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
endif()

# dconfig
file(GLOB DCONFIG_FILES "misc/configs/*.json")
dtk_add_config_meta_files(APPID org.deepin.dde.control-center BASE misc/configs FILES ${DCONFIG_FILES})
file(GLOB DCONFIG_FILE_REGION_FORMAT "misc/configs/common/org.deepin.region-format.json")
dtk_add_config_meta_files(COMMONID true FILES ${DCONFIG_FILE_REGION_FORMAT})

include_directories(
    include
)

set(Test_Libraries
    -lgcov
    Threads::Threads
    GTest::gtest
    Qt::Test
)
# set()
#--------------------------frame--------------------------
set(DCC_Library dde-control-center_frame)
set(DCCFrame_Name ${DCC_Library})
file(GLOB DCCFrame_SRCS
    "include/*.h"
    "src/frame/*.cpp"
)

add_library(${DCCFrame_Name} SHARED
    ${DCCFrame_SRCS}
)

target_include_directories(${DCCFrame_Name} PRIVATE
    include
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
        EXPORT DdeControlCenterInterfaceTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dde-control-center
)

install(EXPORT DdeControlCenterInterfaceTargets
        FILE DdeControlCenterInterfaceTargets.cmake
        NAMESPACE Dde::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter
)
#--------------------------qml-plugin--------------------------
add_subdirectory(src/frame/plugin)
#--------------------------dde-control-center--------------------------
set(Control_Center_Name dde-control-center)
file(GLOB_RECURSE Control_Center_SRCS
    "src/dde-control-center/*.h"
    "src/dde-control-center/*.cpp"
)

add_executable(${Control_Center_Name}
    ${Control_Center_SRCS}
    src/dde-control-center/qrc/dcc.qrc
)

target_compile_definitions(${Control_Center_Name} PRIVATE CVERSION="${CMAKE_PROJECT_VERSION}")

target_include_directories(${Control_Center_Name} PRIVATE
    include/interface
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

file(GLOB_RECURSE qmlFiles  "qml/*.qml")
# file(COPY ${qmlFiles} DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
add_custom_target(${Control_Center_Name}_qml ALL
    SOURCES ${qmlFiles}
)
add_custom_command(TARGET ${Control_Center_Name}_qml
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/qml" ${DCC_LIBDIR}
)
# file(GLOB pluginFiles  "qml/plugins/*")
# file(COPY ${pluginFiles} DESTINATION s${DCC_PLUGINS_INSTALL_DIR})
dcc_handle_plugin_translation(NAME ${Control_Center_Name})
# bin
install(TARGETS ${Control_Center_Name} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/qml/" DESTINATION ${DCC_INSTALL_DIR})

#----------------------------install config------------------------------
# qm files
# file(GLOB QM_FILES "translations/*.qm")
# install(FILES ${QM_FILES} DESTINATION ${DCC_TRANSLATION_INSTALL_DIR})

#desktop
install(FILES misc/org.deepin.dde.control-center.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)

#service
configure_file(
    ${CMAKE_SOURCE_DIR}/misc/org.deepin.dde.ControlCenter1.service.in
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
file(GLOB HEADERS "include/*")
set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/dde-control-center)
install(FILES ${HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR})

configure_package_config_file(misc/DdeControlCenterConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter
    PATH_VARS INCLUDE_INSTALL_DIR
    INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter)

install(FILES "${PROJECT_SOURCE_DIR}/misc/DdeControlCenterPluginMacros.cmake" DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/misc/org.deepin.dde.controlcenter.metainfo.xml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)

#dde-grand-search-daemon conf
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/misc/org.deepin.dde-grand-search.dde-control-center-setting.conf
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/dde-grand-search-daemon/plugins/searcher)

#--------------------------plugins--------------------------
add_subdirectory(src/plugin-sound)
add_subdirectory(src/plugin-defaultapp)
