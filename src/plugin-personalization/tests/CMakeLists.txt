# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

dcc_add_unit_test(ut-personalization
    ut_keyfile.cpp
    ut_wallpaperprovider.cpp
    ../operation/wallpaperprovider.cpp
    ../operation/personalizationdbusproxy.cpp
    ../operation/lastoremanagerjobdbusproxy.cpp
    ../operation/personalizationmodel.cpp
    ../operation/model/wallpapermodel.cpp
    ../operation/model/thememodel.cpp
    ../operation/model/fontmodel.cpp
    ../operation/model/fontsizemodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/keyfile.cpp
)
target_include_directories(ut-personalization PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation
)
target_link_libraries(ut-personalization PRIVATE
    ${DTK_NS}::Gui
    ${QT_NS}::DBus
    ${QT_NS}::Concurrent
    dcc-wallpaper-utils
    dcc-shared-utils
)
