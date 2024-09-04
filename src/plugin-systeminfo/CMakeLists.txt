cmake_minimum_required(VERSION 3.18)
set(SystemInfo_Name systemInfo)
file(GLOB_RECURSE systemInfo_SRCS
        "operation/*.cpp"
        "operation/*.h"
        "operation/qrc/systeminfo.qrc"
)

add_library(${SystemInfo_Name} MODULE
        ${systemInfo_SRCS}
        operation/utils.h
)

set(SystemInfo_Libraries
        ${QT_NS}::DBus
        ${QT_NS}::Widgets
        ${Interface_Name}
        ${DTK_NS}::Gui
)

target_link_libraries(${SystemInfo_Name} PRIVATE
        ${SystemInfo_Libraries}
)

dcc_install_plugin(NAME ${SystemInfo_Name} TARGET ${SystemInfo_Name})
