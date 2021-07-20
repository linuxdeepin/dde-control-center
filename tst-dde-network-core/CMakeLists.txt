cmake_minimum_required(VERSION 3.7)

project(tst-dde-network)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_PREFIX_PATH $ENV{Qt5_DIR})
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core DBus Network REQUIRED)
find_package(GTest REQUIRED)
find_package(KF5NetworkManagerQt REQUIRED)
find_package(Qt5Widgets REQUIRED)

get_target_property(KF5_QT_INCLUDE_DIRS KF5::NetworkManagerQt INTERFACE_INCLUDE_DIRECTORIES)

foreach(LETTER ${TRY_KF5_LIBRARIES})
    if (NOT KF5_QT_LIBRARIES)
        get_target_property(KF5_QT_LIBRARIES KF5::NetworkManagerQt ${LETTER})
    endif()
endforeach()

# 用于测试覆盖率的编译条件
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D SANITIZER_CHECK)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fsanitize=address -O2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address -O2")
endif()

if (NOT KF5_QT_LIBRARIES)
    find_library(KF5_QT_LIBRARIES libKF5NetworkManagerQt.so)
endif()

set(TRY_KF5_LIBRARIES
IMPORTED_LOCATION_DEBIAN
IMPORTED_LOCATION_NOCONFIG
)

aux_source_directory(. FILES)
aux_source_directory(../dde-network-core NETWORKDIR)

add_executable(${PROJECT_NAME} ${FILES} ${NETWORKDIR})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${DFrameworkDBus_INCLUDE_DIRS}
    ${QGSettings_INCLUDE_DIRS}
    ${LibNM_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
    ${Qt5Widget_INCLUDE_DIRS}
    ${KF5_QT_INCLUDE_DIRS}
    ../dde-network-core)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${LibNM_LIBRARIES}
    ${Qt5Network_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Test_LIBRARIES}
    ${Qt5Network_LIBRARIES}
    ${GTEST_LIBRARIES}
    ${QGSettings_LIBRARIES}
    ${KF5_QT_LIBRARIES}
    -lpthread
    -lm)
