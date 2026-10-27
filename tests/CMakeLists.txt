# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Framework-level tests only. Module unit tests live in <module>/tests and are
# registered by their own CMakeLists under UNITTEST. This directory currently
# holds the aggregate coverage target.

# ---- 覆盖率报告生成 target（产物输出 build 目录）
# 使用 lcov + genhtml 生成报告，仅纳入本次被测源文件目录。
# 运行： cmake --build build --target coverage
# 产物： ${CMAKE_BINARY_DIR}/coverage_report/index.html
find_program(LCOV_BIN lcov)
find_program(GENHTML_BIN genhtml)

if(NOT (LCOV_BIN AND GENHTML_BIN))
    message(WARNING "lcov/genhtml not found: the coverage target will only run the tests")
endif()

# lcov 通过 gcov 读取 .gcda/.gcno：GCC 的产物 gcov 直接可读，Clang 的 gcov 兼容
# 产物需要 `llvm-cov gcov`。lcov 的 --gcov-tool 只接受单个可执行路径，所以这里
# 生成一个一行的 wrapper 脚本。
set(_lcov_gcov_tool "")
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    find_program(LLVM_COV_BIN llvm-cov)
    if(LLVM_COV_BIN)
        set(_gcov_wrapper "${CMAKE_BINARY_DIR}/llvm-cov-gcov")
        file(WRITE "${_gcov_wrapper}" "#!/bin/sh\nexec \"${LLVM_COV_BIN}\" gcov \"$@\"\n")
        execute_process(COMMAND chmod +x "${_gcov_wrapper}")
        set(_lcov_gcov_tool --gcov-tool "${_gcov_wrapper}")
    else()
        message(WARNING "Clang build without llvm-cov: the coverage target cannot read Clang's gcov data")
    endif()
endif()

set(_coverage_report_cmds "")
if(LCOV_BIN AND GENHTML_BIN)
    set(_coverage_report_cmds
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/coverage_report"
        COMMAND ${LCOV_BIN} --capture --directory "${CMAKE_BINARY_DIR}"
                --rc lcov_branch_coverage=1
                ${_lcov_gcov_tool}
                --output-file "${CMAKE_BINARY_DIR}/coverage.info"
        COMMAND ${LCOV_BIN} --extract "${CMAKE_BINARY_DIR}/coverage.info"
                --rc lcov_branch_coverage=1
                "${PROJECT_SOURCE_DIR}/src/shared-utils/*"
                "${PROJECT_SOURCE_DIR}/src/plugin-keyboard/operation/*"
                "${PROJECT_SOURCE_DIR}/src/plugin-datetime/operation/timezoneMap/*"
                "${PROJECT_SOURCE_DIR}/src/plugin-defaultapp/operation/*"
                "${PROJECT_SOURCE_DIR}/src/plugin-personalization/operation/*"
                "${PROJECT_SOURCE_DIR}/src/plugin-mouse/operation/*"
                "${PROJECT_SOURCE_DIR}/src/plugin-bluetooth/operation/*"
                "${PROJECT_SOURCE_DIR}/src/plugin-sound/operation/*"
                "${PROJECT_SOURCE_DIR}/src/plugin-dock/operation/*"
                --output-file "${CMAKE_BINARY_DIR}/coverage_filtered.info"
        # Module tests live under src/*/tests and must not count as covered code.
        COMMAND ${LCOV_BIN} --remove "${CMAKE_BINARY_DIR}/coverage_filtered.info" "*/tests/*"
                --rc lcov_branch_coverage=1
                --output-file "${CMAKE_BINARY_DIR}/coverage_final.info"
        COMMAND ${GENHTML_BIN} "${CMAKE_BINARY_DIR}/coverage_final.info"
                --branch-coverage
                --output-directory "${CMAKE_BINARY_DIR}/coverage_report"
    )
endif()

# Build every module test without pulling in the plugins: --target unit-tests
add_custom_target(unit-tests
    DEPENDS
        ut-shared-utils
        ut-keyboard
        ut-datetime
        ut-defaultapp
        ut-personalization
        ut-mouse
        ut-bluetooth
        ut-sound
        ut-dock
)

# Build then run every test: --target run-tests
add_custom_target(run-tests
    COMMAND ${CMAKE_CTEST_COMMAND} --test-dir "${CMAKE_BINARY_DIR}" --output-on-failure
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    DEPENDS unit-tests
    COMMENT "Run all module unit tests"
    VERBATIM
)

add_custom_target(coverage
    ${_coverage_report_cmds}
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    DEPENDS run-tests
    COMMENT "Run all tests and generate coverage report"
    VERBATIM
)
