cmake_minimum_required(VERSION 3.13)

set(DVERSION "6.0.44" CACHE STRING "define project version")
set(BUILD_DOCS ON CACHE BOOL "Generate doxygen-based documentation")

set(PROJECT_NAME dde-control-center-old)
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
option(BUILD_TESTING "UNIT test" OFF)

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

# asan 自己有内存泄露，暂不使用
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # set(UNITTEST ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror=return-type -fno-omit-frame-pointer -Wextra")
    if(ENABLE_ASAN)
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU"  OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -g")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
            set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
            set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
            add_definitions(-DUSE_ASAN)
        endif()
    endif()
else()
    # generate qm
    execute_process(COMMAND bash "misc/translate_generation.sh"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    # generate desktop translate
    execute_process(COMMAND bash "misc/translate_ts2desktop.sh"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
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

set(TRANSLATE_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/dde-control-center/translations" CACHE STRING "Install dir for dde-control-center translate files")

add_definitions(-DTRANSLATE_READ_DIR="${CMAKE_INSTALL_PREFIX}/${TRANSLATE_INSTALL_DIR}")
set(MODULE_INSTALL_DIR "${CMAKE_INSTALL_FULL_LIBDIR}/dde-control-center/modules" CACHE STRING "Install dir for dde-control-center modules")

set(DCC_MODULE_READ_DIR "${MODULE_INSTALL_DIR}" CACHE STRING "Dir to find dde-control-center modules")
add_definitions(-DDCC_DefaultModuleDirectory="${DCC_MODULE_READ_DIR}")

set(LOCALSTATE_READ_DIR "${CMAKE_INSTALL_FULL_LOCALSTATEDIR}" CACHE STRING "Dir to find modifiable single-machine data")
add_definitions(-DVARDIRECTORY="${LOCALSTATE_READ_DIR}")
# Find the library
find_package(PkgConfig REQUIRED)
find_package(Dtk COMPONENTS Core Widget DConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Network DBus Concurrent Multimedia Svg Test REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
# pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "sw_64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
endif()

# dconfig
# file(GLOB DCONFIG_FILES "misc/configs/*.json")
# dtk_add_config_meta_files(APPID org.deepin.dde.control-center BASE misc/configs FILES ${DCONFIG_FILES})
# file(GLOB DCONFIG_FILE_REGION_FORMAT "misc/configs/common/org.deepin.region-format.json")
# dtk_add_config_meta_files(COMMONID true FILES ${DCONFIG_FILE_REGION_FORMAT})

include_directories(
    include
)

set(Test_Libraries
    -lgcov
    Threads::Threads
    GTest::gtest
    Qt::Test
)

#--------------------------interface--------------------------
set(Interface_Name dcc-interface)
file(GLOB_RECURSE Interface_SRCS
    "include/interface/*.h"
    "src/interface/*.cpp"
)
add_library(${Interface_Name} SHARED
    ${Interface_SRCS}
)

set(Interface_Libraries
    Qt::Widgets
    Dtk::Widget
)

target_include_directories(${Interface_Name} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/interface>
)

target_include_directories(${Interface_Name} INTERFACE
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/dde-control-center/>
)

target_link_libraries(${Interface_Name} PUBLIC
    ${Interface_Libraries}
)

set_target_properties(${Interface_Name} PROPERTIES
    VERSION 6
    SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR}
    EXPORT_NAME DCCInterface
)

# so
install(TARGETS ${Interface_Name}
        EXPORT DdeControlCenterInterfaceTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(EXPORT DdeControlCenterInterfaceTargets
        FILE DdeControlCenterInterfaceTargets.cmake
        NAMESPACE Dde::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter
)


#--------------------------dccwidgets library----------------------------
set(Dcc_Widgets_Name dcc-widgets)
file(GLOB_RECURSE Dcc_Widgets_SRCS
    "include/widgets/*.h"
    "src/widgets/*.cpp"
)
add_library(${Dcc_Widgets_Name} SHARED
    ${Dcc_Widgets_SRCS}
)
set(Dcc_Widgets_Includes
    include/widgets
)

set(Dcc_Widgets_Libraries_public
    Dtk::Widget
    Qt::Widgets
    ${Interface_Name}
)

set(Dcc_Widgets_Libraries_private
    Qt::Svg
    Qt::DBus
)

set(Dcc_Widgets_Libraries
    ${Dcc_Widgets_Libraries_public}
    ${Dcc_Widgets_Libraries_private}
)

target_include_directories(${Dcc_Widgets_Name} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/widgets>
)

target_include_directories(${Dcc_Widgets_Name} INTERFACE
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/dde-control-center/>
)

target_link_libraries(${Dcc_Widgets_Name} PUBLIC
    ${Dcc_Widgets_Libraries_public}
)

target_link_libraries(${Dcc_Widgets_Name} PRIVATE
    ${Dcc_Widgets_Libraries_private}
)

set_target_properties(${Dcc_Widgets_Name} PROPERTIES
    VERSION 6
    SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR}
    EXPORT_NAME DCCWidget
)

# so
install(TARGETS ${Dcc_Widgets_Name}
        EXPORT DdeControlCenterWidgetTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(EXPORT DdeControlCenterWidgetTargets
        FILE DdeControlCenterWidgetTargets.cmake
        NAMESPACE Dde::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter
)


#------------------------dccwidgets library test--------------------------
if (UNITTEST)
    set(UT_Dcc_Widgets_Name ut-dcc-widgets)
    file(GLOB_RECURSE UT_Dcc_Widgets_SRCS
        "tests/widgets/*.cpp"
    )
    add_executable(${UT_Dcc_Widgets_Name}
        ${Dcc_Widgets_SRCS}
        ${UT_Dcc_Widgets_SRCS}
    )
    # 用于测试覆盖率的编译条件
    target_compile_options(${UT_Dcc_Widgets_Name} PRIVATE ${UT_COMPILER})
    target_include_directories(${UT_Dcc_Widgets_Name} PUBLIC
        ${Dcc_Widgets_Includes}
    )
    target_link_libraries(${UT_Dcc_Widgets_Name} PRIVATE
        ${Dcc_Widgets_Libraries}
        ${Test_Libraries}
    )
    gtest_discover_tests(${UT_Dcc_Widgets_Name} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

#--------------------------dde-control-center-Agent--------------------------
set(DCC_AGENT_PREFIX "")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(OLD_DCC_SO_PATH ${PROJECT_BINARY_DIR}/../dcc-old/libdcc-widgets.so)
    set(NEW_DCC_SO_PATH ${PROJECT_BINARY_DIR}/../lib/libdde-control-center.so)
    set(OLD_DCC_PATH ${PROJECT_BINARY_DIR}/../dcc-old/libdde-control-center-old.so)
    set(NEW_DCC_PATH ${PROJECT_BINARY_DIR}/../lib/libdde-control-center-lib.so)
else()
    set(DCC_AGENT_PREFIX "/usr/")
    set(OLD_DCC_SO_PATH ${CMAKE_INSTALL_LIBDIR}/libdcc-widgets.so)
    set(NEW_DCC_SO_PATH ${CMAKE_INSTALL_LIBDIR}/libdde-control-center.so)
    set(OLD_DCC_PATH ${CMAKE_INSTALL_LIBDIR}/dde-control-center/libdde-control-center-old.so)
    set(NEW_DCC_PATH ${CMAKE_INSTALL_LIBDIR}/dde-control-center/libdde-control-center-lib.so)
    set(DCC_TRANSLATE_READ_DIR ${CMAKE_INSTALL_PREFIX}/${DCC_TRANSLATION_INSTALL_DIR})
endif()
add_definitions(-DOLD_DCC_SO_PATH="${DCC_AGENT_PREFIX}${OLD_DCC_SO_PATH}")
add_definitions(-DNEW_DCC_SO_PATH="${DCC_AGENT_PREFIX}${NEW_DCC_SO_PATH}")
add_definitions(-DOLD_DCC_PATH="${DCC_AGENT_PREFIX}${OLD_DCC_PATH}")
add_definitions(-DNEW_DCC_PATH="${DCC_AGENT_PREFIX}${NEW_DCC_PATH}")


set(DCC_Agent dde-control-center-agent)
set(DCCAgent_Name ${DCC_Agent})
file(GLOB DCCAgent_SRCS
    "src/dcc-agent/*.c"
)

add_executable(${DCCAgent_Name}
    ${DCCAgent_SRCS}
)

set_target_properties(${DCCAgent_Name} PROPERTIES
    OUTPUT_NAME dde-control-center
)

install(TARGETS ${DCCAgent_Name} DESTINATION ${CMAKE_INSTALL_BINDIR})

#--------------------------dde-control-center--------------------------
set(Control_Center_Name dde-control-center-old)
file(GLOB_RECURSE Control_Center_SRCS
    "src/frame/*.h"
    "src/frame/*.cpp"
)
list(REMOVE_ITEM Control_Center_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/frame/main.cpp")

add_library(${Control_Center_Name} SHARED
    ${Control_Center_SRCS}
    src/frame/main.cpp
)

target_compile_definitions(${Control_Center_Name} PRIVATE CVERSION="${CVERSION}")

set(Control_Center_Libraries
    ${Dcc_Widgets_Name}
    ${Interface_Name}
    Dtk::Widget
    Qt::Widgets
    Qt::DBus
    Qt::Concurrent
)

target_link_libraries(${Control_Center_Name} PRIVATE
    ${Control_Center_Libraries}
)

# bin
install(TARGETS ${Control_Center_Name} DESTINATION ${CMAKE_INSTALL_LIBDIR}/dde-control-center/)

#----------------------------install config------------------------------
# qm files
file(GLOB QM_FILES "translations/*.qm")
install(FILES ${QM_FILES} DESTINATION ${TRANSLATE_INSTALL_DIR})

#desktop
# install(FILES misc/org.deepin.dde.control-center.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)

#service
# configure_file(
#     ${CMAKE_CURRENT_SOURCE_DIR}/misc/org.deepin.dde.ControlCenter1.service.in
#     ${CMAKE_CURRENT_BINARY_DIR}/org.deepin.dde.ControlCenter1.service
#     @ONLY)
# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.deepin.dde.ControlCenter1.service
#         DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/dbus-1/services)

# systemd service
if (NOT DEFINED SYSTEMD_USER_UNIT_DIR)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(Systemd REQUIRED systemd)
    pkg_get_variable(SYSTEMD_USER_UNIT_DIR systemd systemduserunitdir)
endif()

# configure_file(
#     ${CMAKE_SOURCE_DIR}/misc/systemd/dde-control-center.service.in
#     ${CMAKE_CURRENT_BINARY_DIR}/org.deepin.dde.control-center.service
#     @ONLY)

# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.deepin.dde.control-center.service
#         DESTINATION ${SYSTEMD_USER_UNIT_DIR})

# dev files
file(GLOB HEADERS "include/*")
set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/dde-control-center)
install(DIRECTORY ${HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR})

# configure_package_config_file(misc/DdeControlCenterConfig.cmake.in
#     ${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfig.cmake
#     INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter
#     PATH_VARS INCLUDE_INSTALL_DIR
#     INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

# write_basic_package_version_file(
#     "${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfigVersion.cmake"
#     VERSION ${PROJECT_VERSION}
#     COMPATIBILITY SameMajorVersion
# )

# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfig.cmake
#               ${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfigVersion.cmake
#         DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter)

# install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/misc/org.deepin.dde.controlcenter.metainfo.xml
#         DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)

#dde-grand-search-daemon conf
# install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/misc/org.deepin.dde-grand-search.dde-control-center-setting.conf
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/dde-grand-search-daemon/plugins/searcher)
#-------------------------ut-dcc-interface-------------------------
if (UNITTEST)
    set(UT_Interface_Name ut-dcc-interface)
    file(GLOB_RECURSE UT_Interface_SRCS
        "tests/interface/*.cpp"
    )

    set(Interface_Includes
        include/interface
    )

    add_executable(${UT_Interface_Name}
        ${Interface_SRCS}
        ${UT_Interface_SRCS}
    )

    # 用于测试覆盖率的编译条件
    target_compile_options(${UT_Interface_Name} PRIVATE ${UT_COMPILER})

    target_include_directories(${UT_Interface_Name} PUBLIC
        ${Interface_Includes}
    )

    target_link_libraries(${UT_Interface_Name} PRIVATE
        ${Interface_Libraries}
        ${Test_Libraries}
    )
endif()
#-------------------------ut-dcc-frame-------------------------
if (UNITTEST)
    set(UT_Frame_Name ut-dcc-frame)
    file(GLOB_RECURSE UT_Frame_SRCS
        "tests/frame/*.cpp"
    )

    add_executable(${UT_Frame_Name}
        ${Control_Center_SRCS}
        ${UT_Frame_SRCS}
    )

    # 用于测试覆盖率的编译条件
    target_compile_options(${UT_Frame_Name} PRIVATE ${UT_COMPILER})

    target_link_libraries(${UT_Frame_Name} PRIVATE
        ${Control_Center_Libraries}
        ${Test_Libraries}
    )
endif()
#--------------------------plugin-test1--------------------------
if (UNITTEST)
    set(Plugin_Test1_Name plugin-test1)
    file(GLOB_RECURSE Plugin_Test1_SRCS
        "tests/plugin-test1/*.h"
        "tests/plugin-test1/*.cpp"
    )

    add_library(${Plugin_Test1_Name} MODULE
        ${Plugin_Test1_SRCS}
    )

    set(Plugin_Test1_Libraries
        ${Interface_Name}
        Qt::Widgets
        Qt::DBus
        Dtk::Widget
    )

    target_link_libraries(${Plugin_Test1_Name} PRIVATE
        ${Plugin_Test1_Libraries}
    )
endif()

#--------------------------plugin-test2--------------------------
if (UNITTEST)
    set(Plugin_Test2_Name plugin-test2)
    file(GLOB_RECURSE Plugin_Test2_SRCS
        "tests/plugin-test2/*.h"
        "tests/plugin-test2/*.cpp"
    )

    add_library(${Plugin_Test2_Name} MODULE
        ${Plugin_Test2_SRCS}
    )

    set(Plugin_Test2_Includes
        Qt5::Widgets
        Qt5::DBus
    )
    set(Plugin_Test2_Libraries
        ${Interface_Name}
        Qt5::Widgets
        Qt5::DBus
    )
    target_include_directories(${Plugin_Test2_Name} PUBLIC
        ${Plugin_Test2_Includes}
    )

    target_link_libraries(${Plugin_Test2_Name} PRIVATE
        ${Plugin_Test2_Libraries}
    )
endif()

#--------------------------plugin-systeminfo--------------------------
if (BUILD_PLUGIN)
    set(SystemInfo_Name dcc-systeminfo-plugin)
    file(GLOB_RECURSE SYSTEMINFO_SRCS
        "src/plugin-systeminfo/window/*.cpp"
        "src/plugin-systeminfo/operation/*.cpp"
        "src/plugin-systeminfo/operation/qrc/systeminfo.qrc"
    )

    add_library(${SystemInfo_Name} MODULE
        ${SYSTEMINFO_SRCS}
    )

    set(SystemInfo_Includes
        src/plugin-systeminfo

    )
    set(SystemInfo_Libraries
        ${Dcc_Widgets_Name}
        ${Interface_Name}
        Dtk::Widget
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
    )


    target_include_directories(${SystemInfo_Name} PUBLIC
        ${SystemInfo_Includes}
    )

    target_link_libraries(${SystemInfo_Name} PRIVATE
        ${SystemInfo_Libraries}
    )

    install(TARGETS ${SystemInfo_Name} DESTINATION ${CMAKE_INSTALL_LIBDIR}/dde-control-center/modules)
endif()
#-------------------------ut-dcc-systeminfo-plugin-------------------------
if (UNITTEST)
    set(UT_SystemInfo_Name ut-dcc-systeminfo-plugin)
    file(GLOB_RECURSE UT_SYSTEMINFO_SRCS
        "tests/plugin-systeminfo/*.cpp"
    )

    add_executable(${UT_SystemInfo_Name}
        ${SYSTEMINFO_SRCS}
        ${UT_SYSTEMINFO_SRCS}
    )

    # 用于测试覆盖率的编译条件
    target_compile_options(${UT_SystemInfo_Name} PRIVATE ${UT_COMPILER})

    target_include_directories(${UT_SystemInfo_Name} PUBLIC
        ${SystemInfo_Includes}
    )

    target_link_libraries(${UT_SystemInfo_Name} PRIVATE
        ${SystemInfo_Libraries}
        ${Test_Libraries}
    )
    gtest_discover_tests(${UT_SystemInfo_Name} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()
#--------------------------dcc-power-plugin--------------------------
if (BUILD_PLUGIN)
    set(Power_Name dcc-power-plugin)
    file(GLOB_RECURSE Power_SRCS
        "src/plugin-power/window/*.cpp"
        "src/plugin-power/operation/*.cpp"
        "src/plugin-power/operation/qrc/power.qrc"
    )

    add_library(${Power_Name} MODULE
        ${Power_SRCS}
    )

    set(Power_Includes
        src/plugin-power/window
        src/plugin-power/operation
    )
    set(Power_Libraries
        ${Interface_Name}
        Dtk::Widget
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
        ${Dcc_Widgets_Name}
    )
    target_include_directories(${Power_Name} PRIVATE
        ${Power_Includes}
    )

    target_link_libraries(${Power_Name} PRIVATE
        ${Power_Libraries}
    )

    install(TARGETS ${Power_Name} DESTINATION ${CMAKE_INSTALL_LIBDIR}/dde-control-center/modules)
endif()
#--------------------------plugin-mouse--------------------------
if (BUILD_PLUGIN)
    set(Mouse_Name dcc-mouse-plugin)
    file(GLOB_RECURSE MOUSE_SRCS
        "src/plugin-mouse/window/*.cpp"
        "src/plugin-mouse/operation/*.cpp"
        "src/plugin-mouse/operation/qrc/mouse.qrc"
    )

    add_library(${Mouse_Name} MODULE
        ${MOUSE_SRCS}
    )

    set(Mouse_Libraries
        ${Dcc_Widgets_Name}
        ${Interface_Name}
        Dtk::Widget
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
    )

    target_include_directories(${Mouse_Name} PUBLIC
        ${Mouse_Includes}
    )

    target_link_libraries(${Mouse_Name} PRIVATE
        ${Mouse_Libraries}
    )

    install(TARGETS ${Mouse_Name} DESTINATION ${MODULE_INSTALL_DIR})
endif()
#-------------------------ut-dcc-mouse-plugin-------------------------
if (UNITTEST)
    set(UT_Mouse_Name ut-dcc-mouse-plugin)
    file(GLOB_RECURSE UT_MOUSE_SRCS
        "tests/plugin-mouse/*.cpp"
    )

    add_executable(${UT_Mouse_Name}
        ${MOUSE_SRCS}
        ${UT_MOUSE_SRCS}
    )

    # 用于测试覆盖率的编译条件
    target_compile_options(${UT_Mouse_Name} PRIVATE ${UT_COMPILER})


    target_link_libraries(${UT_Mouse_Name} PRIVATE
        ${Mouse_Libraries}
        ${Test_Libraries}
    )
    gtest_discover_tests(${UT_Mouse_Name} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()
#--------------------------plugin-commoninfo--------------------------
if (BUILD_PLUGIN)
    set(CommonInfo_Name dcc-commoninfo-plugin)
    pkg_check_modules(DEEPIN_PW_CHECK REQUIRED libdeepin_pw_check)
    file(GLOB_RECURSE COMMONINFO_SRCS
        "src/plugin-commoninfo/window/*.cpp"
        "src/plugin-commoninfo/operation/*.cpp"
        "src/plugin-commoninfo/operation/qrc/commoninfo.qrc"
    )

    add_library(${CommonInfo_Name} MODULE
        ${COMMONINFO_SRCS}
    )
    set(CommonInfo_Includes
        ${DEEPIN_PW_CHECK_INCLUDE_DIRS}
    )
    set(CommonInfo_Libraries
        ${Dcc_Widgets_Name}
        ${Interface_Name}
        ${DEEPIN_PW_CHECK_LIBRARIES}
        Dtk::Widget
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
    )


    target_include_directories(${CommonInfo_Name} PUBLIC
        ${CommonInfo_Includes}
    )

    target_link_libraries(${CommonInfo_Name} PRIVATE
        ${CommonInfo_Libraries}
    )

    install(TARGETS ${CommonInfo_Name} DESTINATION ${MODULE_INSTALL_DIR})
endif()
#-------------------------ut-dcc-commoninfo-plugin-------------------------
if (UNITTEST)
    set(UT_CommonInfo_Name ut-dcc-commoninfo-plugin)
    file(GLOB_RECURSE UT_COMMONINFO_SRCS
        "tests/plugin-commoninfo/*.cpp"
    )

    add_executable(${UT_CommonInfo_Name}
        ${COMMONINFO_SRCS}
        ${UT_COMMONINFO_SRCS}
    )

    # 用于测试覆盖率的编译条件
    target_compile_options(${UT_CommonInfo_Name} PRIVATE ${UT_COMPILER})

    target_include_directories(${UT_CommonInfo_Name} PUBLIC
        ${CommonInfo_Includes}
    )

    target_link_libraries(${UT_CommonInfo_Name} PRIVATE
        ${CommonInfo_Libraries}
        ${Test_Libraries}
    )
    gtest_discover_tests(${UT_Dcc_Widgets_Name} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

#--------------------------dcc-bluetooth-plugin--------------------------
if (BUILD_PLUGIN)
    set(Bluetooth_Name dcc-bluetooth-plugin)
    file(GLOB_RECURSE Bluetooth_SRCS
        "src/plugin-bluetooth/window/*.cpp"
        "src/plugin-bluetooth/operation/*.cpp"
        "src/plugin-bluetooth/operation/qrc/bluetooth.qrc"
    )

    add_library(${Bluetooth_Name} MODULE
        ${Bluetooth_SRCS}
    )

    set(Bluetooth_Includes
        src/plugin-bluetooth/window
        src/plugin-bluetooth/operation
    )
    set(Bluetooth_Libraries
        ${Interface_Name}
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
        Dtk::Widget
        ${Dcc_Widgets_Name}
    )
    target_include_directories(${Bluetooth_Name} PRIVATE
        ${Bluetooth_Includes}
    )

    target_link_libraries(${Bluetooth_Name} PRIVATE
        ${Bluetooth_Libraries}
    )

    install(TARGETS ${Bluetooth_Name} DESTINATION ${MODULE_INSTALL_DIR})
endif()
#--------------------------dcc-defaultapp-plugin--------------------------
if (BUILD_PLUGIN)
    set(DefaultApp_Name dcc-defaultapp-plugin)
    file(GLOB_RECURSE DefaultApp_SRCS
        "src/plugin-defaultapp/window/*.cpp"
        "src/plugin-defaultapp/window/widgets/*.cpp"
        "src/plugin-defaultapp/operation/*.cpp"
        "src/plugin-defaultapp/operation/qrc/defapp.qrc"
    )
    pkg_check_modules(QGSettings REQUIRED IMPORTED_TARGET gsettings-qt)

    add_library(${DefaultApp_Name} MODULE
        ${DefaultApp_SRCS}
    )

    set(DefaultApp_Includes
        src/plugin-defaultapp/window/widgets
        src/plugin-defaultapp/operation
    )
    set(DefaultApp_Libraries
        ${Dcc_Widgets_Name}
        Dtk::Widget
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
        PkgConfig::QGSettings
        ${Dcc_Widgets_Name}
    )
    target_include_directories(${DefaultApp_Name} PUBLIC
        ${DefaultApp_Includes}
    )

    target_link_libraries(${DefaultApp_Name} PRIVATE
        ${DefaultApp_Libraries}
    )

    install(TARGETS ${DefaultApp_Name} DESTINATION ${MODULE_INSTALL_DIR})
endif()
#--------------------------dcc-datetime-plugin--------------------------
if (BUILD_PLUGIN)
    set(Datetime_Name dcc-datetime-plugin)
    file(GLOB_RECURSE Datetime_SRCS
        "src/plugin-datetime/window/*.cpp"
        "src/plugin-datetime/operation/*.cpp"
        "src/plugin-datetime/operation/qrc/datetime.qrc"
    )

    find_package(ICU COMPONENTS i18n uc)

    add_library(${Datetime_Name} MODULE
        ${Datetime_SRCS}
    )

    set(Datetime_Includes
        src/plugin-datetime/window
        src/plugin-datetime/window/widgets
        src/plugin-datetime/operation
    )

    set(Datetime_Libraries
        ${Interface_Name}
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
        Dtk::Widget
        ${Dcc_Widgets_Name}
        ICU::i18n
        ICU::uc
    )

    target_include_directories(${Datetime_Name} PRIVATE
        ${Datetime_Includes}
    )

    target_link_libraries(${Datetime_Name} PRIVATE
        ${Datetime_Libraries}
    )

    install(TARGETS ${Datetime_Name} DESTINATION ${MODULE_INSTALL_DIR})
endif()
#--------------------------plugin-keyboard--------------------------
if (BUILD_PLUGIN)
    set(KeyBoard_Name dcc-keyboard-plugin)
    file(GLOB_RECURSE KEYBOARD_SRCS
        "src/plugin-keyboard/window/*.cpp"
        "src/plugin-keyboard/operation/*.cpp"
        "src/plugin-keyboard/operation/qrc/keyboard.qrc"
    )
    if (DISABLE_LANGUAGE)
        add_definitions(-DDCC_DISABLE_LANUGAGE)
    endif()
    add_library(${KeyBoard_Name} MODULE
        ${KEYBOARD_SRCS}
    )
    set(KeyBoard_Includes
        src/plugin-keyboard
    )
    set(KeyBoard_Libraries
        ${Dcc_Widgets_Name}
        ${Interface_Name}
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
        Dtk::Widget
    )

    target_include_directories(${KeyBoard_Name} PUBLIC
        ${KeyBoard_Includes}
    )

    target_link_libraries(${KeyBoard_Name} PRIVATE
        ${KeyBoard_Libraries}
    )

    install(TARGETS ${KeyBoard_Name} DESTINATION ${MODULE_INSTALL_DIR})
endif()
#-------------------------ut-dcc-keyboard-plugin-------------------------
if (UNITTEST)
    set(UT_KeyBoard_Name ut-dcc-keyboard-plugin)
    file(GLOB_RECURSE UT_KEYBOARD_SRCS
        "tests/plugin-keyboard/*.cpp"
    )

    add_executable(${UT_KeyBoard_Name}
        ${KEYBOARD_SRCS}
        ${UT_KEYBOARD_SRCS}
    )

    # 用于测试覆盖率的编译条件
    target_compile_options(${UT_KeyBoard_Name} PRIVATE ${UT_COMPILER})

    target_include_directories(${UT_KeyBoard_Name} PUBLIC
        ${KeyBoard_Includes}
    )

    target_link_libraries(${UT_KeyBoard_Name} PRIVATE
        ${KeyBoard_Libraries}
        ${Test_Libraries}
    )

    gtest_discover_tests(${UT_KeyBoard_Name} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()
#--------------------------plugin-sound--------------------------
if (BUILD_PLUGIN)
    set(Sound_Name dcc-sound-plugin)
    file(GLOB_RECURSE Sound_SRCS
        "src/plugin-sound/window/*.cpp"
        "src/plugin-sound/operation/*.cpp"
        "src/plugin-sound/operation/qrc/sound.qrc"
    )

    add_library(${Sound_Name} MODULE
        ${Sound_SRCS}
    )

    if (DISABLE_SOUND_ADVANCED)
        target_compile_definitions(${Sound_Name} PUBLIC -DDCC_DISABLE_SOUND_ADVANCED)
    endif()

    set(Sound_Includes
        src/plugin-sound/operation
        src/plugin-sound/window
    )
    set(Sound_Libraries
        ${Dcc_Widgets_Name}
        Dtk::Widget
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
        Qt::Multimedia
        ${Dcc_Widgets_Name}
    )
    target_include_directories(${Sound_Name} PUBLIC
        ${Sound_Includes}
    )

    target_link_libraries(${Sound_Name} PRIVATE
        ${Sound_Libraries}
    )

    install(TARGETS ${Sound_Name} DESTINATION ${MODULE_INSTALL_DIR})
endif()
#--------------------------plugin-authentication--------------------------
if (BUILD_PLUGIN AND NOT DISABLE_AUTHENTICATION)
    message(STATUS "===================================")
    message(STATUS "")
    message(STATUS "WILL BUILD dcc-authentication-plugin")
    message(STATUS "")
    message(STATUS "WILL BUILD dcc-authentication-plugin")
    set(Authentication_Name dcc-authentication-plugin)
    pkg_check_modules(DaReader REQUIRED dareader)

    file(GLOB_RECURSE Authentication_SRCS
        "src/plugin-authentication/window/*.cpp"
        "src/plugin-authentication/operation/*.cpp"
        "src/plugin-authentication/operation/qrc/authentication.qrc"
    )

    add_library(${Authentication_Name} MODULE
        ${Authentication_SRCS}
    )

    set(Authentication_Includes
        ${DaReader_INCLUDE_DIRS}
        src/plugin-authentication/operation
        src/plugin-authentication/window
    )
    set(Authentication_Libraries
        ${Dcc_Widgets_Name}
        Dtk::Widget
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
        ${Dcc_Widgets_Name}
        ${DaReader_LIBRARIES}
    )
    target_include_directories(${Authentication_Name} PUBLIC
        ${Authentication_Includes}
    )

    target_link_libraries(${Authentication_Name} PRIVATE
        ${Authentication_Libraries}
    )

    install(TARGETS ${Authentication_Name} DESTINATION ${MODULE_INSTALL_DIR})
endif()
#--------------------------plugin-accounts--------------------------
if (BUILD_PLUGIN)
    find_package(PolkitQt5-1)
    set(Accounts_Name dcc-accounts-plugin)
    pkg_check_modules(DEEPIN_PW_CHECK libdeepin_pw_check)
    file(GLOB_RECURSE ACCOUNTS_SRCS
        "src/plugin-accounts/window/*.cpp"
        "src/plugin-accounts/operation/*.cpp"
        "src/plugin-accounts/operation/qrc/accounts.qrc"
    )

    add_library(${Accounts_Name} MODULE
        ${ACCOUNTS_SRCS}
    )
    set(Accounts_Includes
        ${DEEPIN_PW_CHECK_INCLUDE_DIRS}

        src/plugin-accounts/
    )
    set(Accounts_Libraries
        ${Dcc_Widgets_Name}
        ${Interface_Name}
        ${DEEPIN_PW_CHECK_LIBRARIES}
        Dtk::Widget
        Qt::Svg
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
        PolkitQt5-1::Agent
    )

    target_include_directories(${Accounts_Name} PUBLIC
        ${Accounts_Includes}
    )

    target_link_libraries(${Accounts_Name} PRIVATE
        ${Accounts_Libraries}
    )

    install(TARGETS ${Accounts_Name} DESTINATION ${MODULE_INSTALL_DIR})
endif()
#--------------------------plugin-display--------------------------
if (BUILD_PLUGIN)

    function(ws_generate_local type input_file output_name)
        pkg_get_variable(WAYLAND_SCANNER wayland-scanner wayland_scanner)
        execute_process(COMMAND ${WAYLAND_SCANNER}
            ${type}-header
            ${input_file}
            ${CMAKE_CURRENT_BINARY_DIR}/${output_name}.h
        )

        execute_process(COMMAND ${WAYLAND_SCANNER}
            public-code
            ${input_file}
            ${CMAKE_CURRENT_BINARY_DIR}/${output_name}.c
        )
    endfunction()

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(WaylandClient REQUIRED IMPORTED_TARGET wayland-client)

    ws_generate_local(client
        ${CMAKE_CURRENT_SOURCE_DIR}/src/plugin-display/wayland/libwayqt/protocols/wlr-output-management-unstable-v1.xml
        wlr-output-management-unstable-v1-client-protocol)

    ws_generate_local(client
        ${CMAKE_CURRENT_SOURCE_DIR}/src/plugin-display/wayland/libwayqt/protocols/treeland-output-management.xml
        treeland-output-management-client-protocol)

    file(GLOB_RECURSE LIBWAYQT_SRCS
        "src/plugin-display/wayland/libwayqt/*.cpp"
        "src/plugin-display/wayland/libwayqt/*.hpp"
    )

    add_library(libwayqt
        OBJECT
        ${LIBWAYQT_SRCS}
        wlr-output-management-unstable-v1-client-protocol.c
        treeland-output-management-client-protocol.c
    )

    target_include_directories(libwayqt
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/src/plugin-display/wayland/libwayqt"
        PRIVATE
            ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    )

    target_link_libraries(libwayqt
        PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        PkgConfig::WaylandClient
    )

    set(Display_Name dcc-display-plugin)
    file(GLOB_RECURSE DISPLAY_SRCS
        "src/plugin-display/window/*.cpp"
        "src/plugin-display/operation/*.cpp"
        "src/plugin-display/operation/qrc/display.qrc"
    )

    add_library(${Display_Name} MODULE
        ${DISPLAY_SRCS}
    )

    set(Display_Libraries
        ${Dcc_Widgets_Name}
        ${Interface_Name}
        Dtk::Widget
        Qt::Svg
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
        $<TARGET_OBJECTS:libwayqt>
        PkgConfig::WaylandClient
    )

    target_include_directories(${Display_Name} PUBLIC
        src/plugin-display
        ${Display_Includes}
        $<TARGET_PROPERTY:libwayqt,INTERFACE_INCLUDE_DIRECTORIES>
    )

    target_link_libraries(${Display_Name} PRIVATE
        ${Display_Libraries}
    )

    install(TARGETS ${Display_Name} DESTINATION ${MODULE_INSTALL_DIR})
endif()
#--------------------------dcc-personalization-plugin--------------------------
if (BUILD_PLUGIN)
    set(Personalization_Name dcc-personalization-plugin)
    file(GLOB_RECURSE Personalization_SRCS
        "src/plugin-personalization/window/*.cpp"
        "src/plugin-personalization/operation/*.cpp"
        "src/plugin-personalization/operation/qrc/personalization.qrc"
    )

    add_library(${Personalization_Name} MODULE
        ${Personalization_SRCS}
    )
    set(Personalization_Includes
        src/plugin-personalization/window
        src/plugin-personalization/operation
    )
    set(Personalization_Libraries
        ${Interface_Name}
        Dtk::Widget
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
        ${Dcc_Widgets_Name}
    )
    target_include_directories(${Personalization_Name} PRIVATE
        ${Personalization_Includes}
    )

    target_link_libraries(${Personalization_Name} PRIVATE
        ${Personalization_Libraries}
    )

    install(TARGETS ${Personalization_Name} DESTINATION ${MODULE_INSTALL_DIR})
    # install(FILES misc/developdocument.html DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/dde-control-center)
endif()
#--------------------------dcc-personalization-dock-plugin--------------------------
if (BUILD_PLUGIN)
    set(Plugin_Personalization_Dock_Name dcc-personalization-dock-plugin)
    file(GLOB_RECURSE Plugin_Personalization_Dock_SRCS
        "src/plugin-personalization-dock/window/*.cpp"
        "src/plugin-personalization-dock/operation/*.cpp"
        "src/plugin-personalization-dock/operation/qrc/resources.qrc"
    )

    add_library(${Plugin_Personalization_Dock_Name} MODULE
        ${Plugin_Personalization_Dock_SRCS}
    )

    set(Plugin_Personalization_Dock_Includes

        src/plugin-personalization-dock/operation
    )
    set(Plugin_Personalization_Dock_Libraries
        ${Dcc_Widgets_Name}
        ${Interface_Name}
        Dtk::Widget
        Qt::Widgets
        Qt::DBus
    )
    target_include_directories(${Plugin_Personalization_Dock_Name} PUBLIC
        ${Plugin_Personalization_Dock_Includes}
    )

    target_link_libraries(${Plugin_Personalization_Dock_Name} PRIVATE
        ${Plugin_Personalization_Dock_Libraries}
    )

    install(TARGETS ${Plugin_Personalization_Dock_Name} DESTINATION ${MODULE_INSTALL_DIR})
endif()
#--------------------------plugin-Update--------------------------
if (BUILD_PLUGIN AND NOT DISABLE_UPDATE)
    set(Update_Name dcc-update-plugin)
    file(GLOB_RECURSE Update_SRCS
        "src/plugin-update/window/*.cpp"
        "src/plugin-update/operation/*.cpp"
        "src/plugin-update/operation/qrc/update.qrc"
    )

    add_library(${Update_Name} MODULE
        ${Update_SRCS}
    )

    set(Update_Includes
        src/plugin-update/operation
        src/plugin-update/window
    )

    set(Update_Libraries
        ${Dcc_Widgets_Name}
        Qt::Network
        Dtk::Widget
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
        Qt::Multimedia
        ${Dcc_Widgets_Name}
    )

    target_include_directories(${Update_Name} PUBLIC
        ${Update_Includes}
    )

    target_link_libraries(${Update_Name} PRIVATE
        ${Update_Libraries}
    )

    install(TARGETS ${Update_Name} DESTINATION ${MODULE_INSTALL_DIR})
endif()

if (UNITTEST AND NOT DISABLE_UPDATE)
    set(UT_Update_Name ut-dcc-update-plugin)
    file(GLOB_RECURSE UT_UPDATE_SRC
        "tests/plugin-update/*.cpp"
    )

    add_executable(${UT_Update_Name}
        ${Update_SRCS}
        ${UT_UPDATE_SRC}
    )

    target_compile_options(${UT_Update_Name} PRIVATE ${UT_COMPILER})

    target_include_directories(${UT_Update_Name} PUBLIC
        ${Update_Includes}
    )

    target_link_libraries(${UT_Update_Name} PRIVATE
        ${Update_Libraries}
        ${Test_Libraries}
    )

    gtest_discover_tests(${UT_Update_Name} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

endif()
#--------------------------plugin-notification--------------------------
if (BUILD_PLUGIN)
    set(Notification_Name dcc-notification-plugin)
    file(GLOB_RECURSE NOTIFICATION_SRCS
        "src/plugin-notification/window/*.cpp"
        "src/plugin-notification/operation/*.cpp"
        "src/plugin-notification/operation/qrc/notification.qrc"
    )

    add_library(${Notification_Name} MODULE
        ${NOTIFICATION_SRCS}
    )

    set(Notification_Libraries
        ${Dcc_Widgets_Name}
        ${Interface_Name}
        Dtk::Widget
        Qt::Svg
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
    )


    target_link_libraries(${Notification_Name} PRIVATE
        ${Notification_Libraries}
    )

    install(TARGETS ${Notification_Name} DESTINATION ${MODULE_INSTALL_DIR})
endif()
#-------------------------ut-dcc-notification-plugin-------------------------

# FIXME: this ut is wrong!!
# if (UNITTEST)
#     set(UT_Notification_Name ut-dcc-notification-plugin)
#     file(GLOB_RECURSE UT_NOTIFICATION_SRCS
#         "tests/plugin-notification/*.cpp"
#     )
#
#     add_executable(${UT_Notification_Name}
#         ${NOTIFICATION_SRCS}
#         ${UT_NOTIFICATION_SRCS}
#     )
#
#     # 用于测试覆盖率的编译条件
#     target_compile_options(${UT_Notification_Name} PRIVATE ${UT_COMPILER})
#
#     target_include_directories(${UT_Notification_Name} PUBLIC
#         ${Notification_Includes}
#     )
#
#     target_link_libraries(${UT_Notification_Name} PRIVATE
#         ${Notification_Libraries}
#         ${Test_Libraries}
#     )
#
#     gtest_discover_tests(${UT_Notification_Name} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
# endif()

#--------------------------plugin-privacy--------------------------
#if (BUILD_PLUGIN)
#    set(Privacy_Name dcc-privacy-plugin)
#    file(GLOB_RECURSE Privacy_SRCS
#        "src/plugin-privacy/window/*.cpp"
#        "src/plugin-privacy/operation/*.cpp"
#        "src/plugin-privacy/operation/qrc/privacy.qrc"
#    )

#    add_library(${Privacy_Name} MODULE
#        ${Privacy_SRCS}
#    )

#    set(Pricacy_Includes
#        ${DtkWidget_INCLUDE_DIRS}
#        Qt5::Widgets
#        Qt5::DBus
#        Qt5::Concurrent
#        src/plugin-privacy/operation
#        src/plugin-privacy/window
#    )
#    set(Privacy_Libraries
#        ${Dcc_Widgets_Name}
#        ${DtkWidget_LIBRARIES}
#        Qt5::Widgets
#        Qt5::DBus
#        Qt5::Concurrent
#        ${Dcc_Widgets_Name}
#    )
#    target_include_directories(${Privacy_Name} PUBLIC
#        ${Pricacy_Includes}
#    )

#    target_link_libraries(${Privacy_Name} PRIVATE
#        ${Privacy_Libraries}
#    )

#    install(TARGETS ${Privacy_Name} DESTINATION ${MODULE_INSTALL_DIR})
#endif()
#--------------------------plugin-touchscreen--------------------------
if (BUILD_PLUGIN)
    set(Touchscreen_Name dcc-touchscreen-plugin)
    file(GLOB_RECURSE TOUCHSCREEN_SRCS
        "src/plugin-touchscreen/*.cpp"
        "src/plugin-touchscreen/operation/qrc/touchscreen.qrc"
    )

    add_library(${Touchscreen_Name} MODULE
        ${TOUCHSCREEN_SRCS}
    )

    set(Touchscreen_Includes
        src/plugin-touchscreen/window
        src/plugin-touchscreen/operation
    )

    set(Touchscreen_Libraries
        ${Dcc_Widgets_Name}
        ${Interface_Name}
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
        Dtk::Widget
    )

    target_include_directories(${Touchscreen_Name} PUBLIC
        ${Touchscreen_Includes}
    )

    target_link_libraries(${Touchscreen_Name} PRIVATE
        ${Touchscreen_Libraries}
    )

    install(TARGETS ${Touchscreen_Name} DESTINATION ${MODULE_INSTALL_DIR})
endif()
#--------------------------plugin-wacom--------------------------
if (BUILD_PLUGIN)
    set(Wacom_Name dcc-wacom-plugin)
    file(GLOB_RECURSE Wacom_SRCS
        "src/plugin-wacom/window/*.cpp"
        "src/plugin-wacom/operation/*.cpp"
        "src/plugin-wacom/operation/qrc/wacom.qrc"
    )

    add_library(${Wacom_Name} MODULE
        ${Wacom_SRCS}
    )

    set(Pricacy_Includes
        src/plugin-wacom/operation
        src/plugin-wacom/window
    )
    set(Wacom_Libraries
        ${Dcc_Widgets_Name}
        Dtk::Widget
        Qt::Widgets
        Qt::DBus
        Qt::Concurrent
        ${Dcc_Widgets_Name}
    )
    target_include_directories(${Wacom_Name} PUBLIC
        ${Pricacy_Includes}
    )

    target_link_libraries(${Wacom_Name} PRIVATE
        ${Wacom_Libraries}
    )

    install(TARGETS ${Wacom_Name} DESTINATION ${MODULE_INSTALL_DIR})
endif()
#--------------------------plugin-adapterv20tov23--------------------------
if (BUILD_PLUGIN)
    set(AdapterV20toV23_Name dcc-adapterv20tov23-plugin)
    file(GLOB_RECURSE ADAPTERV20TOV23_SRCS
        "src/plugin-adapterv20tov23/*.cpp"
    )

    add_library(${AdapterV20toV23_Name} MODULE
        ${ADAPTERV20TOV23_SRCS}
    )

    set(AdapterV20toV23_Libraries
        ${Dcc_Widgets_Name}
        ${Interface_Name}
        Dtk::Widget
        Qt::Widgets
        Qt::DBus
    )


    target_link_libraries(${AdapterV20toV23_Name} PRIVATE
        ${AdapterV20toV23_Libraries}
    )

    install(TARGETS ${AdapterV20toV23_Name} DESTINATION ${MODULE_INSTALL_DIR})
endif()
