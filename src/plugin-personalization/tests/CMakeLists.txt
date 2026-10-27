# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Personalization module test: KeyFile ini/.desktop parser.
dcc_add_unit_test(ut-personalization
    ut_keyfile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/keyfile.cpp
)
target_include_directories(ut-personalization PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../operation)
