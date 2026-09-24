set(BIN_NAME unit-test)

add_executable(${BIN_NAME}
    ut_pinyinsearch.cpp
    ut_category.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin-keyboard/operation/pinyinsearch.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin-defaultapp/operation/category.cpp
)

target_include_directories(${BIN_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/src/plugin-keyboard/operation
    ${PROJECT_SOURCE_DIR}/src/plugin-defaultapp/operation
)

target_link_libraries(${BIN_NAME} PRIVATE
    GTest::gtest_main
    ${DTK_NS}::Core
    ${QT_NS}::Core
    ${QT_NS}::Test
)

include(GoogleTest)
gtest_discover_tests(${BIN_NAME})
