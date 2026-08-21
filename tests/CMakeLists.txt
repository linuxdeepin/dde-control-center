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
# DockPluginSortProxyModel::lessThan). Applied per-file via set_source_files_properties
# rather than target-wide so existing tests and Qt6 headers (which use private/
# protected inheritance) are unaffected.
set_source_files_properties(
    ut_keyboardmodel.cpp
    ut_dockpluginsortproxymodel.cpp
    ut_categorymodel.cpp
    PROPERTIES COMPILE_DEFINITIONS "protected=public;private=public"
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

# ---- 覆盖率编译插桩（与项目 UT_COMPILER 保持一致）
# 在 Debug+UNITTEST 构建下，主 CMakeLists.txt 已定义 UT_COMPILER
#（GNU: -fprofile-arcs -ftest-coverage；Clang: -fprofile-instr-generate -ftest-coverage）。
# 这里将其应用到 unit-test target，使被测源与测试源都被插桩。
if(UNITTEST AND DEFINED UT_COMPILER)
    target_compile_options(${BIN_NAME} PRIVATE ${UT_COMPILER})
    target_link_options(${BIN_NAME} PRIVATE ${UT_COMPILER})
endif()

# ---- 覆盖率报告生成 target（产物输出 build 目录）
# 使用 gcovr 生成单页自包含 HTML 报告，仅纳入本次被测文件。
# target 自包含：DEPENDS 编译出插桩二进制，前置 ctest 运行产出 .gcda，
# 再由 gcovr 读取 .gcda/.gcno 生成报告。
# 运行： cmake --build build --target coverage
# 产物： ${CMAKE_BINARY_DIR}/coverage_report/coverage-dde-control-center.html
# 注意：--filter 必须用绝对路径——gcovr 的 RelativeFilter 以 CWD（此处为
# ${CMAKE_BINARY_DIR}）为基准而非 --root，相对 "src/..." 在 build/ 下匹配不到
# 任何文件会导致空报告。
find_package(Python3 QUIET COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    set(GCOVR_CMD ${Python3_EXECUTABLE} -m gcovr)
else()
    find_program(GCOVR_PYTHON python3)
    if(GCOVR_PYTHON)
        set(GCOVR_CMD ${GCOVR_PYTHON} -m gcovr)
    else()
        set(GCOVR_CMD gcovr)
    endif()
endif()

set(COVERAGE_HTML_OUT "${CMAKE_BINARY_DIR}/coverage_report/coverage-dde-control-center.html")
add_custom_target(coverage
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/coverage_report"
    COMMAND ${CMAKE_CTEST_COMMAND} --test-dir "${CMAKE_BINARY_DIR}" --output-on-failure
    COMMAND ${GCOVR_CMD}
            --root "${PROJECT_SOURCE_DIR}"
            --filter "${PROJECT_SOURCE_DIR}/src/shared-utils/dcclocale\.cpp"
            --filter "${PROJECT_SOURCE_DIR}/src/plugin-keyboard/operation/metadata\.cpp"
            --filter "${PROJECT_SOURCE_DIR}/src/plugin-datetime/operation/timezoneMap/timezone_map_util\.cpp"
            --filter "${PROJECT_SOURCE_DIR}/src/plugin-defaultapp/operation/category\.cpp"
            --filter "${PROJECT_SOURCE_DIR}/src/plugin-personalization/operation/keyfile\.cpp"
            --filter "${PROJECT_SOURCE_DIR}/src/plugin-mouse/operation/gesturedata\.cpp"
            --filter "${PROJECT_SOURCE_DIR}/src/plugin-bluetooth/operation/bluetoothdevice\.cpp"
            --filter "${PROJECT_SOURCE_DIR}/src/plugin-sound/operation/port\.cpp"
            --filter "${PROJECT_SOURCE_DIR}/src/plugin-sound/operation/soundDeviceData\.cpp"
            --filter "${PROJECT_SOURCE_DIR}/src/plugin-keyboard/operation/keyboardmodel\.cpp"
            --filter "${PROJECT_SOURCE_DIR}/src/plugin-dock/operation/dockpluginsortproxymodel\.cpp"
            --filter "${PROJECT_SOURCE_DIR}/src/plugin-defaultapp/operation/categorymodel\.cpp"
            --html-details
            --html-single-page
            --html-self-contained
            --html-title "DDE-104 控制中心测试覆盖率报告"
            --output "${COVERAGE_HTML_OUT}"
            --print-summary
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    DEPENDS ${BIN_NAME}
    COMMENT "Generating gcovr HTML coverage report -> ${COVERAGE_HTML_OUT}"
    VERBATIM
)
