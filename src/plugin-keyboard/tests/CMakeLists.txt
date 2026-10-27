# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Keyboard module tests: pinyin search, layout metadata, layout model.
dcc_add_unit_test(ut-keyboard
    ut_pinyinsearch.cpp
    ut_metadata.cpp
    ut_keyboardmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/pinyinsearch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/metadata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/keyboardmodel.cpp
)
target_include_directories(ut-keyboard PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../operation)
target_link_libraries(ut-keyboard PRIVATE ${DTK_NS}::Core)

# Direct access to private members (KeyboardModel::convertLang) without the
# `#define private public` macro, which breaks gtest/libstdc++ headers.
set_source_files_properties(ut_keyboardmodel.cpp PROPERTIES COMPILE_OPTIONS "-fno-access-control")
