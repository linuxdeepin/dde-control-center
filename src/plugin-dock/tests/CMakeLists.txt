# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Dock module test: plugin sort proxy model.
dcc_add_unit_test(ut-dock
    ut_dockpluginsortproxymodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/dockpluginsortproxymodel.cpp
)
target_include_directories(ut-dock PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../operation)

set_source_files_properties(ut_dockpluginsortproxymodel.cpp PROPERTIES COMPILE_OPTIONS "-fno-access-control")
