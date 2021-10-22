cmake_minimum_required(VERSION 3.7)

set(BIN_NAME dccwidgets-unittest)

# 自动生成moc文件
set(CMAKE_AUTOMOC ON)

# 源文件
file(GLOB_RECURSE SRCS "*.cpp")

# 用于测试覆盖率的编译条件
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")

 # 增加安全编译参数
 set(SECURITY_COMPILE, "-fstack-protector-strong -D_FORTITY_SOURCE=1 -z noexecstack -pie -fPIC -z lazy")
 set(CMAKE_CXX_FLAGS, "${CMAKE_CXX_FLAGS} ${SECURITY_COMPILE}")
 set(CMAKE_C_FLAGS, "${CMAKE_C_FLAGS} ${SECURITY_COMPILE}")

# Address Sanitizer 内存错误检测工具,打开下面的编译选项可以看到调试信息，正常运行时不需要这些信息
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
 set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fsanitize=address -O2")
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address -O2")
endif()

# 查找依赖库
find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Test DBus REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(GTest REQUIRED)

pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)

# 添加执行文件信息
add_executable(${BIN_NAME} ${SRCS} ${INTERFACES})

# 链接库
target_link_libraries(${BIN_NAME} PRIVATE
    dccwidgets
    ${Qt5Test_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${DtkWidget_LIBRARIES}
    ${GTEST_LIBRARIES}
    -lpthread
    -lm
)

target_include_directories(${BIN_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${DtkWidget_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)
