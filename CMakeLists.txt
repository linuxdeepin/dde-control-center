cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME dde-control-center)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "-g -Wall")

# 增加安全编译参数
ADD_DEFINITIONS("-fstack-protector-strong -D_FORTITY_SOURCE=1 -z noexecstack -pie -fPIC -z lazy")

# asan 自己有内存泄露，暂不使用
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lasan -O -g -fsanitize=address -fno-omit-frame-pointer")
endif()

if (DEFINED ENABLE_MIEEE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
endif()

# Install settings
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif ()

if (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

    # generate qm
    execute_process(COMMAND bash "translate_generation.sh"
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif ()

# Find the library
find_package(PkgConfig REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(Qt5 COMPONENTS Widgets DBus Concurrent Svg Test REQUIRED)

# pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "sw_64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
endif()

# dconfig
file(GLOB DCONFIG_FILES "misc/configs/dde.control-center*.json")
dconfig_meta_files(APPID dde-control-center BASE misc/configs FILES ${DCONFIG_FILES})

include_directories(
    include
)

set(Test_Libraries
    -lpthread
    -lgcov
    -lgtest
    Qt5::Test
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

set(Interface_Includes
    Qt5::Widgets
)
set(Interface_Libraries
    Qt5::Widgets
)
target_include_directories(${Interface_Name} PUBLIC
    ${Interface_Includes}
)

target_link_libraries(${Interface_Name} PRIVATE
    ${Interface_Libraries}
)

# so
install(TARGETS ${Interface_Name} DESTINATION lib)


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
    ${DtkWidget_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)
set(Dcc_Widgets_Libraries
    ${DtkWidget_LIBRARIES}
    Qt5::Widgets
    Qt5::Svg
)
target_include_directories(${Dcc_Widgets_Name} PUBLIC
    ${Dcc_Widgets_Includes}
)
target_link_libraries(${Dcc_Widgets_Name} PRIVATE
    ${Dcc_Widgets_Libraries}
)

# so
install(TARGETS ${Dcc_Widgets_Name} DESTINATION lib)

#------------------------dccwidgets library test--------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(UT_Dcc_Widgets_Name ut-dcc-widgets)
    file(GLOB_RECURSE UT_Dcc_Widgets_SRCS
        "tests/widgets/*.cpp"
    )
    add_executable(${UT_Dcc_Widgets_Name}
        ${Dcc_Widgets_SRCS}
        ${UT_Dcc_Widgets_SRCS}
    )
    # 用于测试覆盖率的编译条件
    target_compile_options(${UT_Dcc_Widgets_Name} PRIVATE -fprofile-arcs -ftest-coverage)
    target_include_directories(${UT_Dcc_Widgets_Name} PUBLIC
        ${Dcc_Widgets_Includes}
    )
    target_link_libraries(${UT_Dcc_Widgets_Name} PRIVATE
        ${Dcc_Widgets_Libraries}
        ${Test_Libraries}
    )
endif()

#--------------------------dde-control-center--------------------------
set(Control_Center_Name dde-control-center)
file(GLOB_RECURSE Control_Center_SRCS
    "src/frame/*.h"
    "src/frame/*.cpp"
)
list(REMOVE_ITEM Control_Center_SRCS "${CMAKE_SOURCE_DIR}/src/frame/main.cpp")

add_executable(${Control_Center_Name}
    ${Control_Center_SRCS}
    src/frame/main.cpp
)

target_compile_definitions(${Control_Center_Name} PRIVATE CVERSION="${CVERSION}")

set(Control_Center_Includes
    ${DtkWidget_INCLUDE_DIRS}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
)
set(Control_Center_Libraries
    ${Interface_Name}
    ${DtkWidget_LIBRARIES}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
)
target_include_directories(${Control_Center_Name} PUBLIC
    ${Control_Center_Includes}
)

target_link_libraries(${Control_Center_Name} PRIVATE
    ${Control_Center_Libraries}
)

# bin
install(TARGETS ${Control_Center_Name} DESTINATION bin)

#----------------------------install config------------------------------
# qm files
file(GLOB QM_FILES "translations/*.qm")
install(FILES ${QM_FILES} DESTINATION share/${PROJECT_NAME}/translations)

#desktop
install(FILES misc/dde-control-center.desktop DESTINATION share/applications)

#dconfig
file(GLOB DCONFIG_FILES "misc/configs/dde.control-center*.json")
install(FILES ${DCONFIG_FILES} DESTINATION /usr/share/dsg/apps/${PROJECT_NAME}/configs)

#service
install(FILES misc/com.deepin.dde.ControlCenter.service DESTINATION share/dbus-1/services)

# dev files
file(GLOB HEADERS "include/*")
set(INCLUDE_INSTALL_DIR include/dde-control-center)
install(DIRECTORY ${HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR})

include(CMakePackageConfigHelpers)
configure_package_config_file(misc/DdeControlCenterConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter
    PATH_VARS INCLUDE_INSTALL_DIR
    INSTALL_PREFIX /usr)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter)

#-------------------------ut-dcc-interface-------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(UT_Interface_Name ut-dcc-interface)
    file(GLOB_RECURSE UT_Interface_SRCS
        "tests/interface/*.cpp"
    )

    add_executable(${UT_Interface_Name}
        ${Interface_SRCS}
        ${UT_Interface_SRCS}
    )

    # 用于测试覆盖率的编译条件
    target_compile_options(${UT_Interface_Name} PRIVATE -fprofile-arcs -ftest-coverage)

    target_include_directories(${UT_Interface_Name} PUBLIC
        ${Interface_Includes}
    )

    target_link_libraries(${UT_Interface_Name} PRIVATE
        ${Interface_Libraries}
        ${Test_Libraries}
    )
endif()
#-------------------------ut-dcc-frame-------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(UT_Frame_Name ut-dcc-frame)
    file(GLOB_RECURSE UT_Frame_SRCS
        "tests/frame/*.cpp"
    )

    add_executable(${UT_Frame_Name}
        ${Control_Center_SRCS}
        ${UT_Frame_SRCS}
    )

    # 用于测试覆盖率的编译条件
    target_compile_options(${UT_Frame_Name} PRIVATE -fprofile-arcs -ftest-coverage)

    target_include_directories(${UT_Frame_Name} PUBLIC
        ${Control_Center_Includes}
    )

    target_link_libraries(${UT_Frame_Name} PRIVATE
        ${Control_Center_Libraries}
        ${Test_Libraries}
    )
endif()
#--------------------------plugin-test--------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(Plugin_Test_Name plugin-test)
    file(GLOB_RECURSE Plugin_Test_SRCS
        "src/plugin-test/*.h"
        "src/plugin-test/*.cpp"
    )

    add_library(${Plugin_Test_Name} MODULE
        ${Plugin_Test_SRCS}
    )

    set(Plugin_Test_Includes
        ${DtkWidget_INCLUDE_DIRS}
        Qt5::Widgets
        Qt5::DBus
    )
    set(Plugin_Test_Libraries
        ${Interface_Name}
        ${DtkWidget_LIBRARIES}
        Qt5::Widgets
        Qt5::DBus
    )
    target_include_directories(${Plugin_Test_Name} PUBLIC
        ${Plugin_Test_Includes}
    )

    target_link_libraries(${Plugin_Test_Name} PRIVATE
        ${Plugin_Test_Libraries}
    )
endif()
#--------------------------plugin-systeminfo--------------------------
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
    ${DtkWidget_INCLUDE_DIRS}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
)
set(SystemInfo_Libraries
    ${Dcc_Widgets_Name}
    ${Interface_Name}
    ${DtkWidget_LIBRARIES}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
)


target_include_directories(${SystemInfo_Name} PUBLIC
    ${SystemInfo_Includes}
)

target_link_libraries(${SystemInfo_Name} PRIVATE
    ${SystemInfo_Libraries}
)
#-------------------------ut-dcc-systeminfo-plugin-------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(UT_SystemInfo_Name ut-dcc-systeminfo-plugin)
    file(GLOB_RECURSE UT_SYSTEMINFO_SRCS
        "tests/plugin-systeminfo/*.cpp"
    )

    add_executable(${UT_SystemInfo_Name}
        ${SYSTEMINFO_SRCS}
        ${UT_SYSTEMINFO_SRCS}
    )

    # 用于测试覆盖率的编译条件
    target_compile_options(${UT_SystemInfo_Name} PRIVATE -fprofile-arcs -ftest-coverage)

    target_include_directories(${UT_SystemInfo_Name} PUBLIC
        ${SystemInfo_Includes}
    )

    target_link_libraries(${UT_SystemInfo_Name} PRIVATE
        ${SystemInfo_Libraries}
        ${Test_Libraries}
    )
endif()
#--------------------------dcc-power-plugin--------------------------
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
    ${DtkWidget_INCLUDE_DIRS}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
    src/plugin-power/window
    src/plugin-power/operation
)
set(Power_Libraries
    ${Interface_Name}
    ${DtkWidget_LIBRARIES}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
    ${Dcc_Widgets_Name}
)
target_include_directories(${Power_Name} PRIVATE
    ${Power_Includes}
)

target_link_libraries(${Power_Name} PRIVATE
    ${Power_Libraries}
)
#--------------------------plugin-mouse--------------------------
set(Mouse_Name dcc-mouse-plugin)
file(GLOB_RECURSE MOUSE_SRCS
    "src/plugin-mouse/window/*.cpp"
    "src/plugin-mouse/operation/*.cpp"
    "src/plugin-mouse/operation/qrc/mouse.qrc"
)

add_library(${Mouse_Name} MODULE
    ${MOUSE_SRCS}
)

set(Mouse_Includes
    ${DtkWidget_INCLUDE_DIRS}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
)

set(Mouse_Libraries
    ${Dcc_Widgets_Name}
    ${Interface_Name}
    ${DtkWidget_LIBRARIES}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
)

target_include_directories(${Mouse_Name} PUBLIC
    ${Mouse_Includes}
)

target_link_libraries(${Mouse_Name} PRIVATE
    ${Mouse_Libraries}
)
#-------------------------ut-dcc-mouse-plugin-------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(UT_Mouse_Name ut-dcc-mouse-plugin)
    file(GLOB_RECURSE UT_MOUSE_SRCS
        "tests/plugin-mouse/*.cpp"
    )

    add_executable(${UT_Mouse_Name}
        ${MOUSE_SRCS}
        ${UT_MOUSE_SRCS}
    )

    # 用于测试覆盖率的编译条件
    target_compile_options(${UT_Mouse_Name} PRIVATE -fprofile-arcs -ftest-coverage)

    target_include_directories(${UT_Mouse_Name} PUBLIC
        ${Mouse_Includes}
    )

    target_link_libraries(${UT_Mouse_Name} PRIVATE
        ${Mouse_Libraries}
        ${Test_Libraries}
    )
endif()
#--------------------------plugin-commoninfo--------------------------
set(CommonInfo_Name dcc-commoninfo-plugin)
set(DEEPIN_PW_CHECK libdeepin_pw_check.so)
file(GLOB_RECURSE COMMONINFO_SRCS
    "src/plugin-commoninfo/window/*.cpp"
    "src/plugin-commoninfo/operation/*.cpp"
    "src/plugin-commoninfo/operation/qrc/commoninfo.qrc"
)

add_library(${CommonInfo_Name} MODULE
    ${COMMONINFO_SRCS}
)
set(CommonInfo_Includes
    ${DtkWidget_INCLUDE_DIRS}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
)
set(CommonInfo_Libraries
    ${Dcc_Widgets_Name}
    ${Interface_Name}
    ${DtkWidget_LIBRARIES}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
    ${DEEPIN_PW_CHECK}
    ${LIBS}
)


target_include_directories(${CommonInfo_Name} PUBLIC
    ${CommonInfo_Includes}
)

target_link_libraries(${CommonInfo_Name} PRIVATE
    ${CommonInfo_Libraries}
)

#--------------------------dcc-bluetooth-plugin--------------------------
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
    ${DtkWidget_INCLUDE_DIRS}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
    src/plugin-bluetooth/window
    src/plugin-bluetooth/operation
)
set(Bluetooth_Libraries
    ${Interface_Name}
    ${DtkWidget_LIBRARIES}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
    ${Dcc_Widgets_Name}
)
target_include_directories(${Bluetooth_Name} PRIVATE
    ${Bluetooth_Includes}
)

target_link_libraries(${Bluetooth_Name} PRIVATE
    ${Bluetooth_Libraries}
)
