cmake_minimum_required(VERSION 3.18)
set(Power_Name power)
file(GLOB_RECURSE power_SRCS
    "operation/*.cpp"
    "operation/*.h"
    "operation/qrc/power.qrc"
)

add_library(${Power_Name} MODULE
    ${power_SRCS}
)

set(Power_Libraries
    ${DCC_FRAME_Library}
    ${DTK_NS}::Gui
    ${QT_NS}::DBus
    ${QT_NS}::Concurrent
)

target_link_libraries(${Power_Name} PRIVATE
    ${Power_Libraries}
)

dcc_install_plugin(NAME ${Power_Name} TARGET ${Power_Name})
