cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME dde-control-center)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "-g -Wall")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(UNITTEST ON)
endif()

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
find_package(Qt5 COMPONENTS Widgets DBus Concurrent Multimedia Svg Test REQUIRED)

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
if (UNITTEST)
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
    target_compile_options(${UT_Frame_Name} PRIVATE -fprofile-arcs -ftest-coverage)

    target_include_directories(${UT_Frame_Name} PUBLIC
        ${Control_Center_Includes}
    )

    target_link_libraries(${UT_Frame_Name} PRIVATE
        ${Control_Center_Libraries}
        ${Test_Libraries}
    )
endif()
#--------------------------plugin-test1--------------------------
if (UNITTEST)
    set(Plugin_Test1_Name plugin-test1)
    file(GLOB_RECURSE Plugin_Test1_SRCS
        "src/plugin-test1/*.h"
        "src/plugin-test1/*.cpp"
    )

    add_library(${Plugin_Test1_Name} MODULE
        ${Plugin_Test1_SRCS}
    )

    set(Plugin_Test1_Includes
        Qt5::Widgets
        Qt5::DBus
    )
    set(Plugin_Test1_Libraries
        ${Interface_Name}
        Qt5::Widgets
        Qt5::DBus
    )
    target_include_directories(${Plugin_Test1_Name} PUBLIC
        ${Plugin_Test1_Includes}
    )

    target_link_libraries(${Plugin_Test1_Name} PRIVATE
        ${Plugin_Test1_Libraries}
    )
endif()

#--------------------------plugin-test2--------------------------
if (UNITTEST)
    set(Plugin_Test2_Name plugin-test2)
    file(GLOB_RECURSE Plugin_Test2_SRCS
        "src/plugin-test2/*.h"
        "src/plugin-test2/*.cpp"
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

install(TARGETS ${SystemInfo_Name} DESTINATION lib/dde-control-center/modules)

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

install(TARGETS ${Power_Name} DESTINATION lib/dde-control-center/modules)
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

install(TARGETS ${Mouse_Name} DESTINATION lib/dde-control-center/modules)
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
pkg_check_modules(DEEPIN_PW_CHECK deepin_pw_check)
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
    ${DEEPIN_PW_CHECK_INCLUDE_DIRS}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
)
set(CommonInfo_Libraries
    ${Dcc_Widgets_Name}
    ${Interface_Name}
    ${DtkWidget_LIBRARIES}
    ${DEEPIN_PW_CHECK_LIBRARIES}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
)


target_include_directories(${CommonInfo_Name} PUBLIC
    ${CommonInfo_Includes}
)

target_link_libraries(${CommonInfo_Name} PRIVATE
    ${CommonInfo_Libraries}
)

install(TARGETS ${CommonInfo_Name} DESTINATION lib/dde-control-center/modules)
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
    target_compile_options(${UT_CommonInfo_Name} PRIVATE -fprofile-arcs -ftest-coverage)

    target_include_directories(${UT_CommonInfo_Name} PUBLIC
        ${CommonInfo_Includes}
    )

    target_link_libraries(${UT_CommonInfo_Name} PRIVATE
        ${CommonInfo_Libraries}
        ${Test_Libraries}
    )
endif()

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

install(TARGETS ${Bluetooth_Name} DESTINATION lib/dde-control-center/modules)

#--------------------------dcc-defaultapp-plugin--------------------------
set(DefaultApp_Name dcc-defaultapp-plugin)
file(GLOB_RECURSE DefaultApp_SRCS
    "src/plugin-defaultapp/window/*.cpp"
    "src/plugin-defaultapp/window/widgets/*.cpp"
    "src/plugin-defaultapp/operation/*.cpp"
    "src/plugin-defaultapp/operation/qrc/defapp.qrc"
)

add_library(${DefaultApp_Name} MODULE
    ${DefaultApp_SRCS}
)

set(DefaultApp_Includes
    ${DtkWidget_INCLUDE_DIRS}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
    src/plugin-defaultapp/window/widgets
    src/plugin-defaultapp/operation
)
set(DefaultApp_Libraries
    ${Dcc_Widgets_Name}
    ${DtkWidget_LIBRARIES}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
    ${Dcc_Widgets_Name}
)
target_include_directories(${DefaultApp_Name} PUBLIC
    ${DefaultApp_Includes}
)

target_link_libraries(${DefaultApp_Name} PRIVATE
    ${DefaultApp_Libraries}
)

install(TARGETS ${DefaultApp_Name} DESTINATION lib/dde-control-center/modules)

#--------------------------dcc-datetime-plugin--------------------------
set(Datetime_Name dcc-datetime-plugin)
file(GLOB_RECURSE Datetime_SRCS
    "src/plugin-datetime/window/*.cpp"
    "src/plugin-datetime/operation/*.cpp"
    "src/plugin-datetime/operation/qrc/datetime.qrc"
)

add_library(${Datetime_Name} MODULE
    ${Datetime_SRCS}
)

set(Datetime_Includes
    ${DtkWidget_INCLUDE_DIRS}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
    src/plugin-datetime/window
    src/plugin-datetime/window/widgets
    src/plugin-datetime/operation
)

set(Datetime_Libraries
    ${Interface_Name}
    ${DtkWidget_LIBRARIES}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
    ${Dcc_Widgets_Name}
)

target_include_directories(${Datetime_Name} PRIVATE
    ${Datetime_Includes}
)

target_link_libraries(${Datetime_Name} PRIVATE
    ${Datetime_Libraries}
)

install(TARGETS ${Datetime_Name} DESTINATION lib/dde-control-center/modules)
#--------------------------plugin-keyboard--------------------------
set(KeyBoard_Name dcc-keyboard-plugin)
file(GLOB_RECURSE KEYBOARD_SRCS
    "src/plugin-keyboard/window/*.cpp"
    "src/plugin-keyboard/operation/*.cpp"
    "src/plugin-keyboard/operation/qrc/keyboard.qrc"
)

add_library(${KeyBoard_Name} MODULE
    ${KEYBOARD_SRCS}
)
set(KeyBoard_Includes
    ${DtkWidget_INCLUDE_DIRS}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
    src/plugin-keyboard
)
set(KeyBoard_Libraries
    ${Dcc_Widgets_Name}
    ${Interface_Name}
    ${DtkWidget_LIBRARIES}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
)

target_include_directories(${KeyBoard_Name} PUBLIC
    ${KeyBoard_Includes}
)

target_link_libraries(${KeyBoard_Name} PRIVATE
    ${KeyBoard_Libraries}
)

install(TARGETS ${KeyBoard_Name} DESTINATION lib/dde-control-center/modules)
#--------------------------plugin-sound--------------------------
set(Sound_Name dcc-sound-plugin)
file(GLOB_RECURSE Sound_SRCS
    "src/plugin-sound/window/*.cpp"
    "src/plugin-sound/operation/*.cpp"
    "src/plugin-sound/operation/qrc/sound.qrc"
)

add_library(${Sound_Name} MODULE
    ${Sound_SRCS}
)

set(Sound_Includes
    ${DtkWidget_INCLUDE_DIRS}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
    src/plugin-sound/operation
    src/plugin-sound/window
)
set(Sound_Libraries
    ${Dcc_Widgets_Name}
    ${DtkWidget_LIBRARIES}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
    ${Qt5Multimedia_LIBRARIES}
    ${Dcc_Widgets_Name}
)
target_include_directories(${Sound_Name} PUBLIC
    ${Sound_Includes}
)

target_link_libraries(${Sound_Name} PRIVATE
    ${Sound_Libraries}
)

install(TARGETS ${Sound_Name} DESTINATION lib/dde-control-center/modules)

#--------------------------plugin-authentication--------------------------
set(Authentication_Name dcc-authentication-plugin)
pkg_check_modules(DaReader dareader)

file(GLOB_RECURSE Authentication_SRCS
    "src/plugin-authentication/window/*.cpp"
    "src/plugin-authentication/operation/*.cpp"
    "src/plugin-authentication/operation/qrc/authentication.qrc"
)

add_library(${Authentication_Name} MODULE
    ${Authentication_SRCS}
)

set(Authentication_Includes
    ${DtkWidget_INCLUDE_DIRS}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
    ${DaReader_INCLUDE_DIRS}
    src/plugin-authentication/operation
    src/plugin-authentication/window
)
set(Authentication_Libraries
    ${Dcc_Widgets_Name}
    ${DtkWidget_LIBRARIES}
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
    ${Dcc_Widgets_Name}
    ${DaReader_LIBRARIES}
)
target_include_directories(${Authentication_Name} PUBLIC
    ${Authentication_Includes}
)

target_link_libraries(${Authentication_Name} PRIVATE
    ${Authentication_Libraries}
)

install(TARGETS ${Authentication_Name} DESTINATION lib/dde-control-center/modules)

#--------------------------plugin-accounts--------------------------
find_package(PolkitQt5-1)
set(Accounts_Name dcc-accounts-plugin)
pkg_check_modules(DEEPIN_PW_CHECK deepin_pw_check)
file(GLOB_RECURSE ACCOUNTS_SRCS
    "src/plugin-accounts/window/*.cpp"
    "src/plugin-accounts/operation/*.cpp"
    "src/plugin-accounts/operation/qrc/accounts.qrc"
)

add_library(${Accounts_Name} MODULE
    ${ACCOUNTS_SRCS}
)
set(Accounts_Includes
    ${DtkWidget_INCLUDE_DIRS}
    ${DEEPIN_PW_CHECK_INCLUDE_DIRS}
    Qt5::Svg
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
)
set(Accounts_Libraries
    ${Dcc_Widgets_Name}
    ${Interface_Name}
    ${DtkWidget_LIBRARIES}
    ${DEEPIN_PW_CHECK_LIBRARIES}
    Qt5::Svg
    Qt5::Widgets
    Qt5::DBus
    Qt5::Concurrent
    PolkitQt5-1::Agent
)

target_include_directories(${Accounts_Name} PUBLIC
    ${Accounts_Includes}
)

target_link_libraries(${Accounts_Name} PRIVATE
    ${Accounts_Libraries}
)

install(TARGETS ${Accounts_Name} DESTINATION lib/dde-control-center/modules)
