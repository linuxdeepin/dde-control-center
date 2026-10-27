# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Mouse module test: gesture data object.
dcc_add_unit_test(ut-mouse
    ut_gesturedata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/gesturedata.cpp
)
target_include_directories(ut-mouse PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../operation)
