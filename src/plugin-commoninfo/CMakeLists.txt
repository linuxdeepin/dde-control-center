cmake_minimum_required(VERSION 3.18)
set(CommonInfo_Name commonInfo)
file(GLOB_RECURSE commonInfo_SRCS
    "operation/*.cpp"
    "operation/*.h"
    "operation/qrc/commoninfo.qrc"
)

add_library(${CommonInfo_Name} MODULE
    ${commonInfo_SRCS}
)

set(CommonInfo_Libraries
    dde-control-center
    ${QT_NS}::DBus
    ${DTK_NS}::Gui
)

target_link_libraries(${CommonInfo_Name} PRIVATE
    ${CommonInfo_Libraries}
)

dcc_install_plugin(NAME ${CommonInfo_Name} TARGET ${CommonInfo_Name})
