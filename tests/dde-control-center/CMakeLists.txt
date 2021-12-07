cmake_minimum_required(VERSION 3.7)
project(dde-control-center-unittest)

set(BLUETOOTH_NAME bluetooth-unittest)
set(MOUSE_NAME mouse-unittest)
set(DATETIME_NAME datetime-unittest)
set(NOTIFICATION_NAME notification-unittest)
set(DEFAPP_NAME defapp-unittest)
set(SYSTEMINFO_NAME systeminfo-unittest)
set(KEYBOARD_NAME keyboard-unittest)

# 自动生成moc文件
set(CMAKE_AUTOMOC ON)

# 增加安全编译参数
ADD_DEFINITIONS("-z relro -z now -pie -z noexecstack -fstack-protector-all -D_FORTITY_SOURCE=1")

# Address Sanitizer 内存错误检测工具,打开下面的编译选项可以看到调试信息，正常运行时不需要这些信息
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fsanitize=address -O2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address -O2 -fstack-protector-strong -D_FORTITY_SOURCE=1 -z noexecstack -pie -fPIC -z lazy")
endif()

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
  ../../src/frame/window/gsettingwatcher.cpp
  ../../src/frame/window/modules/notification/notificationwidget.cpp
  ../../src/frame/window/modules/notification/appnotifywidget.cpp
  ../../src/frame/window/modules/notification/notificationitem.cpp
  ../../src/frame/window/modules/notification/systemnotifywidget.cpp
  ../../src/frame/window/modules/notification/timeslotitem.cpp

  fakedbus/notification_dbus.cpp
)

# 默认程序源文件
file(GLOB_RECURSE DEFAPP_SRCS "defapp/*.cpp" "defapp/*.h")

# 默认程序依赖文件
file(GLOB_RECURSE DEFAPP_Tasks_SRCS
  ../../src/frame/modules/defapp/defappworker.cpp
  ../../src/frame/modules/defapp/defappmodel.cpp
  ../../src/frame/modules/defapp/model/category.cpp
  ../../src/frame/window/gsettingwatcher.cpp
  ../../src/frame/window/insertplugin.cpp
  ../../src/frame/widgets/multiselectlistview.cpp

  ../../src/frame/window/modules/defapp/defappdetailwidget.cpp
  ../../src/frame/window/modules/defapp/defappwidget.cpp

  fakedbus/defapp_dbus.cpp
)

# 系统信息模块源文件
file(GLOB_RECURSE SYSTEMINFO_SRCS "systeminfo/*.cpp" "systeminfo/*.h")

# 系统信息依赖文件
file(GLOB_RECURSE SYSTEMINFO_Tasks_SRCS

   #../../src/frame/window/modules/systeminfo/*.cpp
   ../../src/frame/window/modules/systeminfo/privacypolicywidget.cpp
   ../../src/frame/window/modules/systeminfo/systeminfowidget.cpp
   ../../src/frame/window/modules/systeminfo/nativeinfowidget.cpp
   ../../src/frame/window/modules/systeminfo/userlicensewidget.cpp
   ../../src/frame/window/modules/systeminfo/versionprotocolwidget.cpp
   ../../src/frame/modules/systeminfo/*.cpp
   ../../src/frame/window/gsettingwatcher.cpp
   ../../src/frame/window/insertplugin.cpp
   ../../src/frame/window/utils.h
   ../../src/frame/window/protocolfile.cpp
)

# 键盘测试模块源文件
file(GLOB_RECURSE KEYBOARD_SRCS "keyboard/*.cpp")

# 头文件路径
set(KEYBOARD_HEADER 
    ../../src/frame/window/modules/keyboard
    ../../src/frame/modules/keyboard
)

# 键盘测试依赖文件
file(GLOB_RECURSE KEYBOARD_Tasks_SRCS 
    ../../src/frame/window/modules/keyboard/generalkbsettingwidget.cpp
    ../../src/frame/window/modules/keyboard/kblayoutsettingwidget.cpp
    ../../src/frame/window/modules/keyboard/systemlanguagewidget.cpp
    ../../src/frame/window/modules/keyboard/systemlanguagesettingwidget.cpp
    ../../src/frame/window/modules/keyboard/shortcutsettingwidget.cpp
    ../../src/frame/modules/keyboard/keyboardmodel.cpp
    ../../src/frame/modules/keyboard/indexmodel.cpp
    ../../src/frame/modules/keyboard/shortcutmodel.cpp
    ../../src/frame/modules/keyboard/shortcutitem.cpp
    ../../src/frame/modules/keyboard/shortcutkey.cpp
    ../../src/frame/window/gsettingwatcher.cpp
    ../../src/frame/modules/display/displaymodel.cpp
    ../../src/frame/modules/display/monitor.cpp
    ../../src/frame/modules/keyboard/keylabel.cpp
    ../../src/frame/window/utils.h
)

# 用于测试覆盖率的编译条件
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")

# 查找依赖库
find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Test DBus REQUIRED Concurrent Svg)
find_package(DtkWidget REQUIRED)
find_package(GTest REQUIRED)

pkg_check_modules(QGSettings REQUIRED gsettings-qt)
pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)

# 添加蓝牙模块执行文件信息
add_executable(${BLUETOOTH_NAME} ${BLUETOOTH_SRCS} ${BLUETOOTH_Tasks_SRCS})

# 添加时间日期模块执行文件信息
add_executable(${DATETIME_NAME} ${DATETIME_SRCS} ${DATETIME_Tasks_SRCS})

# 添加鼠标模块执行文件信息
add_executable(${MOUSE_NAME} ${MOUSE_SRCS} ${MOUSE_Tasks_SRCS})

# 添加通知模块执行文件信息
add_executable(${NOTIFICATION_NAME} ${NOTIFICATION_SRCS} ${NOTIFICATION_Tasks_SRCS})

# 添加默认程序模块执行文件信息
add_executable(${DEFAPP_NAME} ${DEFAPP_SRCS} ${DEFAPP_Tasks_SRCS})

# 添加系统信息模块执行文件信息
add_executable(${SYSTEMINFO_NAME} ${SYSTEMINFO_SRCS} ${SYSTEMINFO_Tasks_SRCS})

# 添加键盘模块执行文件信息
add_executable(${KEYBOARD_NAME} ${KEYBOARD_SRCS} ${KEYBOARD_Tasks_SRCS})

# 蓝牙模块链接库
target_link_libraries(${BLUETOOTH_NAME} PRIVATE
    dccwidgets
    ${Qt5Test_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${QGSettings_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${DtkWidget_LIBRARIES}
    ${GTEST_LIBRARIES}
    -lpthread
)

# 蓝牙模块引用头文件
target_include_directories(${BLUETOOTH_NAME} PUBLIC
    ${DtkWidget_INCLUDE_DIRS}
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
    ${DtkWidget_LIBRARIES}
    ${GTEST_LIBRARIES}
    -lpthread
)

# 时间日期模块引用头文件
target_include_directories(${DATETIME_NAME} PUBLIC
    ${DtkWidget_INCLUDE_DIRS}
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
    ${DtkWidget_LIBRARIES}
    ${GTEST_LIBRARIES}
    -lpthread
)

# 鼠标模块引用头文件
target_include_directories(${MOUSE_NAME} PUBLIC
    ${DtkWidget_INCLUDE_DIRS}
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
    ${DtkWidget_LIBRARIES}
    ${GTEST_LIBRARIES}
    -lpthread
)

# 通知模块引用头文件
target_include_directories(${NOTIFICATION_NAME} PUBLIC
    ${DtkWidget_INCLUDE_DIRS}
    ${QGSettings_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Concurrent_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
)

# 默认程序模块链接库
target_link_libraries(${DEFAPP_NAME} PRIVATE
    dccwidgets
    ${Qt5Test_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${QGSettings_LIBRARIES}
    ${Qt5Concurrent_LIBRARIES}
    ${Qt5Svg_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${DtkWidget_LIBRARIES}
    ${GTEST_LIBRARIES}
    -lpthread
)

# 默认程序模块引用头文件
target_include_directories(${DEFAPP_NAME} PUBLIC
    ${DtkWidget_INCLUDE_DIRS}
    ${QGSettings_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Concurrent_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
)

# 系统信息模块链接库
target_link_libraries(${SYSTEMINFO_NAME} PRIVATE
    dccwidgets
    ${Qt5Test_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${QGSettings_LIBRARIES}
    ${Qt5Concurrent_LIBRARIES}
    ${Qt5Svg_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${DtkWidget_LIBRARIES}
    ${GTEST_LIBRARIES}
    -lpthread
)

# 键盘模块链接库
target_link_libraries(${KEYBOARD_NAME} PRIVATE
    dccwidgets
    ${Qt5Test_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${QGSettings_LIBRARIES}
    ${Qt5Concurrent_LIBRARIES}
    ${Qt5Svg_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${DtkWidget_LIBRARIES}
    ${GTEST_LIBRARIES}
    -lpthread
)

# 系统信息模块引用头文件
target_include_directories(${SYSTEMINFO_NAME} PUBLIC
    ${DtkWidget_INCLUDE_DIRS}
    ${QGSettings_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Concurrent_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
)

# 键盘模块引用头文件
target_include_directories(${KEYBOARD_NAME} PUBLIC
    ${KEYBOARD_HEADER}
    ${DtkWidget_INCLUDE_DIRS}
    ${QGSettings_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
)

add_custom_target(check
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/dde-control-center)

#'make check'命令依赖与我们的测试程序
add_dependencies(check ${BLUETOOTH_NAME} ${MOUSE_NAME} ${DATETIME_NAME} ${NOTIFICATION_NAME} ${DEFAPP_NAME} ${SYSTEMINFO_NAME} ${KEYBOARD_NAME})

include_directories(../../src/frame)
include_directories(fakedbus)
