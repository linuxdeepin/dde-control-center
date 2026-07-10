# SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

add_library(dcc-shared-utils OBJECT)
set_target_properties(dcc-shared-utils PROPERTIES POSITION_INDEPENDENT_CODE ON)

find_package(ICU COMPONENTS i18n uc)
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEGTHUMBNAILER REQUIRED IMPORTED_TARGET libffmpegthumbnailer)

set(DCC_SHARED_UTILS_SOURCES
    "dcclocale.h"
    "dcclocale.cpp"
)

target_sources(dcc-shared-utils PRIVATE
    ${DCC_SHARED_UTILS_SOURCES}
)

target_include_directories(dcc-shared-utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(dcc-shared-utils PRIVATE
    ICU::i18n
    ICU::uc
    ${QT_NS}::Core
)

add_library(dcc-wallpaper-utils STATIC
    wallpaperthumbnailutils.cpp
)
set_target_properties(dcc-wallpaper-utils PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(dcc-wallpaper-utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(dcc-wallpaper-utils
    PUBLIC
        ${QT_NS}::Core
    PRIVATE
        PkgConfig::FFMPEGTHUMBNAILER
)

# Treeland input manager (wraps treeland_input_manager_v1 Wayland protocol)
if (Enable_TreelandSupport)
    find_package(TreelandProtocols REQUIRED)
    find_package(${QT_NS} REQUIRED COMPONENTS WaylandClient)
    if(${QT_NS}_VERSION VERSION_GREATER_EQUAL 6.10)
        find_package(${QT_NS} COMPONENTS WaylandClientPrivate REQUIRED)
    endif()

    qt6_generate_wayland_protocol_client_sources(dcc-shared-utils
        FILES
            ${TREELAND_PROTOCOLS_DATA_DIR}/treeland-input-manager-unstable-v1.xml
            ${TREELAND_PROTOCOLS_DATA_DIR}/treeland-keyboard-state-notify-unstable-v1.xml
    )

    target_include_directories(dcc-shared-utils PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
    )

    target_sources(dcc-shared-utils PRIVATE
        treelandinputmanager.h
        treelandinputmanager.cpp
        keyboardstatenotify.h
        keyboardstatenotify.cpp
    )

    if(${QT_NS}_VERSION VERSION_GREATER_EQUAL 6.10)
        target_link_libraries(dcc-shared-utils PUBLIC
            ${QT_NS}::WaylandClient
            ${QT_NS}::WaylandClientPrivate
        )
    else()
        target_link_libraries(dcc-shared-utils PUBLIC
            ${QT_NS}::WaylandClient
        )
    endif()

    target_compile_definitions(dcc-shared-utils PUBLIC ENABLE_TREELAND_INPUT_MANAGER)
endif()
