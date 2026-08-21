cmake_minimum_required(VERSION 3.18)
set(BlueTooth_Name blueTooth)
file(GLOB_RECURSE blueTooth_SRCS
    "operation/*.cpp"
    "operation/*.h"
    "operation/qrc/bluetooth.qrc"
)

add_library(${BlueTooth_Name} MODULE
    ${blueTooth_SRCS}
    operation/bluetoothdevicemodel.h operation/bluetoothdevicemodel.cpp
    operation/bluetoothadaptersmodel.h operation/bluetoothadaptersmodel.cpp

)

set(BlueTooth_Libraries
    ${DCC_FRAME_Library}
    ${QT_NS}::DBus
    ${DTK_NS}::Gui
)

target_link_libraries(${BlueTooth_Name} PRIVATE
    ${BlueTooth_Libraries}
)

dcc_install_plugin(NAME ${BlueTooth_Name} TARGET ${BlueTooth_Name})
