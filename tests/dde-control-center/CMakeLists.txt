cmake_minimum_required(VERSION 3.7)
project(dde-control-center-unittest)

set(BLUETOOTH_NAME bluetooth-unittest)
set(MOUSE_NAME mouse-unittest)
set(DATETIME_NAME datetime-unittest)
set(NOTIFICATION_NAME notification-unittest)

# 自动生成moc文件
set(CMAKE_AUTOMOC ON)

# 蓝牙模块源文件
file(GLOB_RECURSE BLUETOOTH_SRCS "bluetooth/*.cpp" "bluetooth/*.h")

#蓝牙模块依赖文件
file(GLOB_RECURSE BLUETOOTH_Tasks_SRCS
  ../../src/frame/modules/bluetooth/*.cpp

  ../../src/frame/window/modules/bluetooth/bluetoothwidget.cpp
  ../../src/frame/window/modules/bluetooth/adapterwidget.cpp
  ../../src/frame/window/modules/bluetooth/titleedit.cpp
  ../../src/frame/window/modules/bluetooth/devicesettingsitem.cpp
  ../../src/frame/window/modules/bluetooth/detailpage.cpp

  ../../src/frame/window/gsettingwatcher.cpp
  fakedbus/bluetooth_dbus.cpp
)

# 时间日期模块源文件
file(GLOB_RECURSE DATETIME_SRCS "datetime/*.cpp" "datetime/*.h")

# 时间日期模块依赖文件
file(GLOB_RECURSE DATETIME_Tasks_SRCS
    ../../src/frame/modules/datetime/datetimework.cpp
    ../../src/frame/modules/datetime/datetimemodel.cpp
    ../../src/frame/modules/datetime/timezoneitem.cpp
    ../../src/frame/modules/datetime/clock.cpp

    fakedbus/datetime_dbus.cpp
)

# 鼠标模块源文件
file(GLOB_RECURSE MOUSE_SRCS "mouse/*.cpp" "mouse/*.h")

# 鼠标模块依赖文件
file(GLOB_RECURSE MOUSE_Tasks_SRCS
  ../../src/frame/modules/mouse/mouseworker.cpp
  ../../src/frame/modules/mouse/mousemodel.cpp
  ../../src/frame/modules/mouse/doutestwidget.cpp
  ../../src/frame/modules/mouse/palmdetectsetting.cpp

  ../../src/frame/window/modules/mouse/mousewidget.cpp
  ../../src/frame/window/modules/mouse/generalsettingwidget.cpp
  ../../src/frame/window/modules/mouse/mousesettingwidget.cpp
  ../../src/frame/window/modules/mouse/touchpadsettingwidget.cpp
  ../../src/frame/window/modules/mouse/trackpointsettingwidget.cpp

  ../../src/frame/window/utils.h
  ../../src/frame/window/insertplugin.cpp
  ../../src/frame/window/gsettingwatcher.cpp
)

#通知模块源文件
file(GLOB_RECURSE NOTIFICATION_SRCS "notification/*.cpp" "notification/*.h")

#通知模块依赖文件
file(GLOB_RECURSE NOTIFICATION_Tasks_SRCS
  ../../src/frame/modules/notification/*.cpp

  ../../src/frame/window/modules/notification/notificationwidget.cpp
  ../../src/frame/window/modules/notification/appnotifywidget.cpp
  ../../src/frame/window/modules/notification/notificationitem.cpp
  ../../src/frame/window/modules/notification/systemnotifywidget.cpp
  ../../src/frame/window/modules/notification/timeslotitem.cpp

  fakedbus/notification_dbus.cpp
)

# 用于测试覆盖率的编译条件
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")

# 查找依赖库
find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Test DBus REQUIRED Concurrent Svg)
find_package(DtkWidget5.5 REQUIRED)
find_package(GTest REQUIRED)

pkg_check_modules(QGSettings REQUIRED gsettings-qt)
pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)

# 添加蓝牙模块执行文件信息
add_executable(${BLUETOOTH_NAME} ${BLUETOOTH_SRCS} ${BLUETOOTH_Tasks_SRCS})

# 添加时间日期模块执行文件信息
add_executable(${DATETIME_NAME} ${DATETIME_SRCS} ${DATETIME_Tasks_SRCS})

# 添加鼠标模块执行文件信息
add_executable(${MOUSE_NAME} ${MOUSE_SRCS} ${MOUSE_Tasks_SRCS})

# 添加鼠标模块执行文件信息
add_executable(${NOTIFICATION_NAME} ${NOTIFICATION_SRCS} ${NOTIFICATION_Tasks_SRCS})

# 蓝牙模块链接库
target_link_libraries(${BLUETOOTH_NAME} PRIVATE
    dccwidgets
    ${Qt5Test_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${QGSettings_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${DtkWidget5.5_LIBRARIES}
    ${GTEST_LIBRARIES}
    -lpthread
)

# 蓝牙模块引用头文件
target_include_directories(${BLUETOOTH_NAME} PUBLIC
    ${DtkWidget5.5_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${QGSettings_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
)

# 时间日期模块链接库
target_link_libraries(${DATETIME_NAME} PUBLIC
    dccwidgets
    ${Qt5Test_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Concurrent_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${DtkWidget5.5_LIBRARIES}
    ${GTEST_LIBRARIES}
    -lpthread
)

# 时间日期模块引用头文件
target_include_directories(${DATETIME_NAME} PUBLIC
    ${DtkWidget5.5_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Concurrent_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
)

# 鼠标模块链接库
target_link_libraries(${MOUSE_NAME} PRIVATE
    dccwidgets
    ${Qt5Test_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${QGSettings_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${DtkWidget5.5_LIBRARIES}
    ${GTEST_LIBRARIES}
    -lpthread
)

# 鼠标模块引用头文件
target_include_directories(${MOUSE_NAME} PUBLIC
    ${DtkWidget5.5_INCLUDE_DIRS}
    ${QGSettings_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
)

# 通知模块链接库
target_link_libraries(${NOTIFICATION_NAME} PRIVATE
    dccwidgets
    ${Qt5Test_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${QGSettings_LIBRARIES}
    ${Qt5Concurrent_LIBRARIES}
    ${Qt5Svg_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${DtkWidget5.5_LIBRARIES}
    ${GTEST_LIBRARIES}
    -lpthread
)

# 通知模块引用头文件
target_include_directories(${NOTIFICATION_NAME} PUBLIC
    ${DtkWidget5.5_INCLUDE_DIRS}
    ${QGSettings_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Concurrent_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
)

if(1)
    add_custom_target(check
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/dde-control-center)

    add_custom_command(TARGET check
        COMMAND echo " =================== CREAT LCOV REPROT BEGIN ==================== "
        COMMAND echo ${CMAKE_BINARY_DIR}/tests/dde-control-center

        #1.清理原先的gcov信息
        COMMAND lcov --directory ./CMakeFiles/${BLUETOOTH_NAME}.dir --zerocounters
        COMMAND lcov --directory ./CMakeFiles/${MOUSE_NAME}.dir --zerocounters
        COMMAND lcov --directory ./CMakeFiles/${DATETIME_NAME}.dir --zerocounters
        COMMAND lcov --directory ./CMakeFiles/${NOTIFICATION_NAME}.dir --zerocounters
        COMMAND ./${BLUETOOTH_NAME}
        COMMAND ./${MOUSE_NAME}
        COMMAND ./${DATETIME_NAME}
        COMMAND ./${NOTIFICATION_NAME}

        #2.收集gcov信息到.info文件中
        COMMAND lcov --directory . --capture --output-file ./coverage.info

        #3.过滤一些我们不感兴趣的文件的覆盖率信息
        COMMAND echo " =================== do filter begin ==================== "
        COMMAND lcov --remove ./coverage.info
        '*/${BLUETOOTH_NAME}_autogen/*' '*/${MOUSE_NAME}_autogen/*' '*/${DATETIME_NAME}_autogen/*' '*/${NOTIFICATION_NAME}_autogen/*' '*/usr/include/*' '*/tests/*' '*/googletest/*'
        -o ./coverage.info
        COMMAND echo " =================== do filter end ==================== "

        #3.将.info信息生成报告到reprot文件夹中
        COMMAND genhtml -o ${CMAKE_BINARY_DIR}/tests/report ./coverage.info

        COMMAND echo " =================== CREAT LCOV REPROT END ==================== "

        COMMAND echo " Coverage files have been output to ${CMAKE_BINARY_DIR}/tests/report "

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/dde-control-center
        )

    #'make check'命令依赖与我们的测试程序
    add_dependencies(check ${BLUETOOTH_NAME} ${MOUSE_NAME} ${DATETIME_NAME} ${NOTIFICATION_NAME})
endif()

include_directories(../../src/frame)
include_directories(fakedbus)
