set(BIN_NAME unit-test)

add_executable(${BIN_NAME}
    ut_pinyinsearch.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin-keyboard/operation/pinyinsearch.cpp
    ut_timezone_map_util.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin-datetime/operation/timezoneMap/timezone_map_util.cpp
)

target_include_directories(${BIN_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/src/plugin-keyboard/operation
    ${PROJECT_SOURCE_DIR}/src/plugin-datetime/operation/timezoneMap
)

target_link_libraries(${BIN_NAME} PRIVATE
    GTest::gtest_main
    ${DTK_NS}::Core
    ${QT_NS}::Core
)

include(GoogleTest)
gtest_discover_tests(${BIN_NAME})
