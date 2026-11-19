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


# 数据通道契约测试：只依赖 defaultappdata 与本模块的 Category/CategoryModel，
# 需要框架库提供 dccV25::DataReply 的定义
dcc_add_unit_test(ut-defaultappdata
    ut_defaultappdata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/defaultappdata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/category.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/categorymodel.cpp
)
target_include_directories(ut-defaultappdata PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation
    ${DCC_PROJECT_ROOT_DIR}/include)
target_link_libraries(ut-defaultappdata PRIVATE ${DCC_FRAME_Library})
set_source_files_properties(ut_categorymodel.cpp PROPERTIES COMPILE_OPTIONS "-fno-access-control")

# Category 读取状态与快照校验：区分"已确定未设置"和"尚未读取"
dcc_add_unit_test(ut-categorystate
    ut_categorystate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/defaultappdata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/category.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/categorymodel.cpp
)
target_include_directories(ut-categorystate PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation
    ${DCC_PROJECT_ROOT_DIR}/include)
target_link_libraries(ut-categorystate PRIVATE ${DCC_FRAME_Library})
