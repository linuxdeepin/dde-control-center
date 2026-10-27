# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Default-app module tests: category tree and its Qt model.
dcc_add_unit_test(ut-defaultapp
    ut_category.cpp
    ut_categorymodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/category.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/categorymodel.cpp
)
target_include_directories(ut-defaultapp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../operation)

set_source_files_properties(ut_categorymodel.cpp PROPERTIES COMPILE_OPTIONS "-fno-access-control")
