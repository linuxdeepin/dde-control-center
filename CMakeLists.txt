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
set(ENABLE_ASAN OFF)
option(BUILD_TESTING "UNIT test" OFF)
set(BUILD_EXAMPLES OFF)

# dcc-v23
set(BUILD_DCC_OLD OFF)
if (BUILD_DCC_OLD)
    add_subdirectory(dcc-old)
endif()

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
# plugin-keyboard: DISABLE_LANGUAGE to disable language panel

option(DISABLE_AUTHENTICATION "disable build authentication plugins" OFF)
option(DISABLE_PRIVACY_PLUGIN "disable privacy and security plugin" OFF)
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
    execute_process(COMMAND bash "misc/translate_generation.sh" "${CMAKE_CURRENT_BINARY_DIR}/src/dde-control-center"
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

set(POSITION_INDEPENDENT_CODE True)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

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

set(DCC_TRANSLATION_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/translations/v1.0" CACHE STRING "Install dir for dde-control-center translate files")

# 输出目录
set(DCC_LIBDIR ${PROJECT_BINARY_DIR}/lib)
# 插件目录
set(DCC_PLUGINS_DIR plugins_v1.0)
# 安装目录
set(DCC_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/dde-control-center)
set(DCC_DEBUG_LIBDIR ${DCC_INSTALL_DIR})
set(DCC_TRANSLATE_READ_DIR ${CMAKE_INSTALL_PREFIX}/${DCC_TRANSLATION_INSTALL_DIR})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DCC_DEBUG_LIBDIR ${DCC_LIBDIR})
    set(DCC_TRANSLATE_READ_DIR ${PROJECT_BINARY_DIR}/src/dde-control-center)
endif()

add_definitions(-DTRANSLATE_READ_DIR="${DCC_TRANSLATE_READ_DIR}")
# 插件安装目录
set(DCC_PLUGINS_INSTALL_DIR ${DCC_DEBUG_LIBDIR}/${DCC_PLUGINS_DIR} CACHE STRING "Install dir for dde-control-center plugins")
# 插件读取目录
set(MODULE_READ_DIR "${DCC_DEBUG_LIBDIR}" CACHE STRING "Dir to find dde-control-center modules")
set(PLUGINS_READ_DIR "${DCC_PLUGINS_INSTALL_DIR}" CACHE STRING "Dir to find dde-control-center plugins")

GNUInstallDirs_get_absolute_install_dir(
    MODULE_READ_FULL_DIR
    MODULE_READ_DIR
    LIBDIR
)

GNUInstallDirs_get_absolute_install_dir(
    PLUGINS_READ_FULL_DIR
    PLUGINS_READ_DIR
    LIBDIR
)

message(STATUS ${MODULE_READ_FULL_DIR})
message(STATUS ${PLUGINS_READ_FULL_DIR})
message(STATUS ${DCC_PLUGINS_INSTALL_DIR})

add_definitions(-DDefaultModuleDirectory="${MODULE_READ_FULL_DIR}")
add_definitions(-DDefaultPluginsDirectory="${PLUGINS_READ_FULL_DIR}")

# DdeControlCenterPluginMacros.cmake 使用
set(DDE_CONTROL_CENTER_PLUGIN_DIR ${DCC_PLUGINS_DIR})
set(DDE_CONTROL_CENTER_TRANSLATION_INSTALL_DIR ${DCC_TRANSLATION_INSTALL_DIR})
set(DDE_CONTROL_CENTER_PLUGIN_INSTALL_DIR ${DCC_PLUGINS_INSTALL_DIR})

set(LOCALSTATE_READ_DIR "${CMAKE_INSTALL_FULL_LOCALSTATEDIR}" CACHE STRING "Dir to find modifiable single-machine data")
set(DCC_PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DVARDIRECTORY="${LOCALSTATE_READ_DIR}")
# Find the library
find_package(PkgConfig REQUIRED)
find_package(${DTK_NS} REQUIRED COMPONENTS Core Gui  )
find_package(${QT_NS} COMPONENTS Core Quick Gui Network DBus Concurrent Test LinguistTools Multimedia REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

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
    ${QT_NS}::Test
)

set(DCC_FRAME_Library dde-control-center_frame)

add_subdirectory(src/dde-control-center)
#--------------------------plugins--------------------------
add_subdirectory(src/plugin-accounts)
if (NOT DISABLE_AUTHENTICATION)
    add_subdirectory(src/plugin-authentication)
endif()
add_subdirectory(src/plugin-sound)
add_subdirectory(src/plugin-defaultapp)
add_subdirectory(src/plugin-power)
add_subdirectory(src/plugin-mouse)
add_subdirectory(src/plugin-personalization)
add_subdirectory(src/plugin-keyboard)
add_subdirectory(src/plugin-systeminfo)
add_subdirectory(src/plugin-commoninfo)
add_subdirectory(src/plugin-touchscreen)
add_subdirectory(src/plugin-datetime)
add_subdirectory(src/plugin-wacom)
add_subdirectory(src/plugin-bluetooth)
add_subdirectory(src/plugin-notification)
add_subdirectory(src/plugin-deepinid)
add_subdirectory(src/plugin-display)
if (NOT DISABLE_PRIVACY_PLUGIN)
    add_subdirectory(src/plugin-privacy)
endif()
add_subdirectory(src/plugin-dock)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(UNITTEST)
    add_subdirectory(tests)
endif()
