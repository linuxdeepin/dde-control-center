# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# 工厂 v2 / 数据通道的框架级测试。
# 约定：需要真实会话总线的用例统一用 dbus-run-session 启动，
# 不用开发者当前的桌面会话。

add_executable(ut-datarequest ut_datarequest.cpp
    ${DCC_PROJECT_ROOT_DIR}/src/dde-control-center/dccdatarequest.cpp)
target_include_directories(ut-datarequest PRIVATE
    ${DCC_PROJECT_ROOT_DIR}/include
    ${DCC_PROJECT_ROOT_DIR}/src/dde-control-center)
target_link_libraries(ut-datarequest PRIVATE
    GTest::gtest
    ${QT_NS}::Core
    ${QT_NS}::DBus
    ${QT_NS}::Qml
    ${DCC_FRAME_Library})
# dccdatarequest.cpp 依赖 dccLog 分类定义在 dccobject_p.h（随框架库导出）
target_link_libraries(ut-datarequest PRIVATE dde-control-center-lib)

find_program(DBUS_RUN_SESSION dbus-run-session)
if(DBUS_RUN_SESSION)
    # 该环境里 dbus-run-session 的 stdout 走管道会挂住；用 shell 重定向到文件规避
    add_test(NAME FactoryDataChannel.ut-datarequest
             COMMAND sh -c "\"${DBUS_RUN_SESSION}\" -- \"$1\" > \"$2\" 2>&1; rc=$?; cat \"$2\"; exit $rc"
                     sh $<TARGET_FILE:ut-datarequest>
                     ${CMAKE_CURRENT_BINARY_DIR}/ut-datarequest.log)
else()
    add_test(NAME FactoryDataChannel.ut-datarequest
             COMMAND $<TARGET_FILE:ut-datarequest>)
endif()

# 该用例依赖私有会话总线；给它明确上限，避免在无桌面环境的 CI 中无限等待
set_tests_properties(FactoryDataChannel.ut-datarequest PROPERTIES TIMEOUT 60)

add_dependencies(unit-tests ut-datarequest)
