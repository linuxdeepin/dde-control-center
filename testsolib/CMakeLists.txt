cmake_minimum_required(VERSION 3.7)

set(VERSION 4.0)

set(BIN_NAME testso)

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "-g -Wall")

# Find the library
find_package(PkgConfig REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(DtkGui REQUIRED)
find_package(DtkCMake REQUIRED)
find_package(Qt5 COMPONENTS
    Core
    Widgets
    DBus
REQUIRED)

pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)

set(Qt_LIBS
    Qt5::Core
    Qt5::Gui
    Qt5::DBus
    Qt5::Widgets
)

add_library(${BIN_NAME} MODULE
    testsomodule.cpp
    testsomodule.h
    testsowidget.cpp
    testsowidget.h
    thirdpage1.cpp
    thirdpage1.h
    resources/testmodule.qrc
)
target_include_directories(${BIN_NAME} PUBLIC
    ../interface
    ${DtkWidget_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)
target_link_libraries(${BIN_NAME} PRIVATE
    ${DFrameworkDBus_LIBRARIES}
    ${DtkWidget_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
)

# 设置安装路径的前缀(默认为"/usr/local")
set(CMAKE_INSTALL_PREFIX "/usr")

# 设置执行 make install 时哪个目标应该被 install 到哪个位置
install(TARGETS ${BIN_NAME} LIBRARY DESTINATION lib/dde-control-center/modules)
