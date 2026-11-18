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

# 数据通道的取值与校验来源：Category 默认项与候选列表。
# 只需要 Category，不链接完整插件（避免拖入 DBus 后端依赖）。
dcc_add_unit_test(ut-defaultappdata
    ut_defaultappdata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/category.cpp
)
target_include_directories(ut-defaultappdata PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../operation)

set_source_files_properties(ut_categorymodel.cpp PROPERTIES COMPILE_OPTIONS "-fno-access-control")
