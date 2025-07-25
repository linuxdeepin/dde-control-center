# SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

add_library(dcc-shared-utils OBJECT)

find_package(ICU COMPONENTS i18n uc)

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
