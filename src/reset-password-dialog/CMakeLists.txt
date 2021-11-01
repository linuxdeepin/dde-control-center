cmake_minimum_required(VERSION 3.7)

set(VERSION 4.0)

set(BIN_NAME reset-password-dialog)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)
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

if(NOT DEFINED DISABLE_SYS_UPDATE)

set(SRCS
        main.cpp
        resetpassworddialog.h
        resetpassworddialog.cpp
)
set(QRC resetpassworddialog.qrc)
# Find the library
find_package(PkgConfig REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(DtkGui REQUIRED)

pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)

add_executable(${BIN_NAME} ${SRCS} ${QRC})
target_include_directories(${BIN_NAME} PUBLIC
    ${DtkWidget_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(${BIN_NAME} PRIVATE
    ${DFrameworkDBus_LIBRARIES}
    ${DtkWidget_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    crypt
    ${LIBS}
)

# bin
install(TARGETS ${BIN_NAME} DESTINATION lib/dde-control-center/)

# configure
install(CODE "execute_process(COMMAND sudo useradd -M pwd_changer)")
install(CODE "execute_process(COMMAND sudo usermod -L pwd_changer)")
install(CODE "execute_process(COMMAND sudo chown pwd_changer:pwd_changer /usr/lib/dde-control-center/${BIN_NAME})")
install(CODE "execute_process(COMMAND sudo chmod 500 /usr/lib/dde-control-center/${BIN_NAME})")

endif()
