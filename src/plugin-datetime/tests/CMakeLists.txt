# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Datetime module test: timezone map projection / nearest-timezone math.
dcc_add_unit_test(ut-datetime
    ut_timezone_map_util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/timezoneMap/timezone_map_util.cpp
)
target_include_directories(ut-datetime PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/timezoneMap
)
