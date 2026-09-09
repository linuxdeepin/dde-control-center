find_package(ICU COMPONENTS i18n uc)

set(BIN_NAME unit-test)

add_executable(${BIN_NAME}
    ut_pinyinsearch.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin-keyboard/operation/pinyinsearch.cpp
    ut_dcclocale.cpp
    ${PROJECT_SOURCE_DIR}/src/shared-utils/dcclocale.cpp
    ut_metadata.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin-keyboard/operation/metadata.cpp
    ut_timezone_map_util.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin-datetime/operation/timezoneMap/timezone_map_util.cpp
    ut_category.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin-defaultapp/operation/category.cpp
    ut_keyfile.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin-personalization/operation/keyfile.cpp
    ut_gesturedata.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin-mouse/operation/gesturedata.cpp
    ut_bluetoothdevice.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin-bluetooth/operation/bluetoothdevice.cpp
    ut_port.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin-sound/operation/port.cpp
    ut_sounddevicedata.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin-sound/operation/soundDeviceData.cpp
    ut_keyboardmodel.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin-keyboard/operation/keyboardmodel.cpp
    ut_dockpluginsortproxymodel.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin-dock/operation/dockpluginsortproxymodel.cpp
    ut_categorymodel.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin-defaultapp/operation/categorymodel.cpp
)

target_include_directories(${BIN_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/src/plugin-keyboard/operation
    ${PROJECT_SOURCE_DIR}/src/shared-utils
    ${PROJECT_SOURCE_DIR}/src/plugin-datetime/operation/timezoneMap
    ${PROJECT_SOURCE_DIR}/src/plugin-defaultapp/operation
    ${PROJECT_SOURCE_DIR}/src/plugin-personalization/operation
    ${PROJECT_SOURCE_DIR}/src/plugin-mouse/operation
    ${PROJECT_SOURCE_DIR}/src/plugin-bluetooth/operation
    ${PROJECT_SOURCE_DIR}/src/plugin-sound/operation
    ${PROJECT_SOURCE_DIR}/src/plugin-dock/operation
)

# Expose protected/private members for the test files that need direct access
# to non-public members (e.g. KeyboardModel::convertLang, CategoryModel::getAppById,
# DockPluginSortProxyModel::lessThan). Applied per-file via -fno-access-control
# (compile OPTION, not the `#define private public` macro which breaks
# gtest/libstdc++ <sstream>/<any>).
set_source_files_properties(
    ut_keyboardmodel.cpp
    ut_dockpluginsortproxymodel.cpp
    ut_categorymodel.cpp
    PROPERTIES COMPILE_OPTIONS "-fno-access-control"
)

target_link_libraries(${BIN_NAME} PRIVATE
    GTest::gtest_main
    ${DTK_NS}::Core
    ${QT_NS}::Core
    ${QT_NS}::Test
    ${QT_NS}::DBus
    ICU::i18n
    ICU::uc
)

include(GoogleTest)
gtest_discover_tests(${BIN_NAME})

# ---- 覆盖率编译插桩（由本 target 自决，不改动上游的 UT_COMPILER）
# 本 target 的覆盖率走 lcov 报告，插桩必须与 gcov 兼容：
#   * GCC 直接产出 .gcno/.gcda；
#   * Clang 用 -fprofile-arcs -ftest-coverage 产出 gcov 兼容数据，读取时经
#     --gcov-tool llvm-cov gcov（见下方 coverage target）。
# 上游 CMakeLists.txt 的 UT_COMPILER 给 Clang 配的是 LLVM 原生
# -fprofile-instr-generate（产出 .profraw，lcov 读不了），故此处不使用它。
if(UNITTEST)
    target_compile_options(${BIN_NAME} PRIVATE -fprofile-arcs -ftest-coverage)
    target_link_options(${BIN_NAME} PRIVATE -fprofile-arcs -ftest-coverage)
endif()

# ---- 覆盖率报告生成 target（产物输出 build 目录）
# 使用 lcov + genhtml 生成报告，仅纳入本次被测源文件目录。
# target 自包含：DEPENDS 编译出插桩二进制，前置 ctest 运行产出 .gcda，
# 再由 lcov --capture 汇总、--extract 只保留被测模块、genhtml 渲染。
# 运行： cmake --build build --target coverage
# 产物： ${CMAKE_BINARY_DIR}/coverage_report/index.html
find_program(LCOV_BIN lcov)
find_program(GENHTML_BIN genhtml)

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
        COMMAND ${GENHTML_BIN} "${CMAKE_BINARY_DIR}/coverage_filtered.info"
                --branch-coverage
                --output-directory "${CMAKE_BINARY_DIR}/coverage_report"
    )
endif()
add_custom_target(coverage
    COMMAND ${CMAKE_CTEST_COMMAND} --test-dir "${CMAKE_BINARY_DIR}" --output-on-failure
    ${_coverage_report_cmds}
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    DEPENDS ${BIN_NAME}
    COMMENT "Run all tests and generate coverage report"
    VERBATIM
)
