cmake_minimum_required(VERSION 3.7)

set(VERSION 4.0)

set(BIN_NAME restore-tool)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "-g -Wall")

if (DEFINED ENABLE_MIEEE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
endif()

# Install settings
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif ()

set(SRCS
    main.cpp
    partitionusage.cpp
    filesystem.cpp
    command.cpp
)

# Find the library
find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Core DBus REQUIRED)
find_package(DtkWidget REQUIRED)

pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)
pkg_search_module(UDisk2 REQUIRED udisks2-qt5)

include_directories(AFTER ${UDisk2_INCLUDE_DIRS})

add_executable(${BIN_NAME} ${SRCS})
target_include_directories(${BIN_NAME} PUBLIC
    ${PROJECT_BINARY_DIR}
    ${DFrameworkDBus_INCLUDE_DIRS}
    ${UDisk2_INCLUDE_DIRS}
    ${DtkWidget_INCLUDE_DIRS}
)

target_link_libraries(${BIN_NAME} PUBLIC
    ${DFrameworkDBus_LIBRARIES}
    Qt5::Core
    Qt5::DBus
    ${UDisk2_LIBRARIES}
    ${DtkWidget_LIBRARIES}
)

# bin
install(TARGETS ${BIN_NAME} DESTINATION /bin)
