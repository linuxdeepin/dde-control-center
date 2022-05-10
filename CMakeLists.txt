cmake_minimum_required(VERSION 3.7)

project(dde-network-core)

# 增加安全编译参数
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-all")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector-all")
set(CMAKE_EXE_LINKER_FLAGS  "-z relro -z now -z noexecstack -pie")

set(DDE-Network-Core_INCLUDE_DIRS  "${CMAKE_SOURCE_DIR}/src")
set(DDE-Network-Core_LIBRARIES  dde-network-core)

add_subdirectory("src")
add_subdirectory("dcc-network-plugin")
add_subdirectory("tests")
add_subdirectory("example")
add_subdirectory("dock-network-plugin")
add_subdirectory("dde-network-dialog")
add_subdirectory("dss-network-plugin")
add_subdirectory("dss_example")
add_subdirectory("dock-example")
