# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

# DCCLocale string-transform tests. dcclocale.cpp is compiled in directly so
# the test binary does not need the full dcc-shared-utils dependency chain.
find_package(ICU COMPONENTS i18n uc REQUIRED)

dcc_add_unit_test(ut-shared-utils
    ut_dcclocale.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../dcclocale.cpp
)
target_include_directories(ut-shared-utils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(ut-shared-utils PRIVATE ICU::i18n ICU::uc)
