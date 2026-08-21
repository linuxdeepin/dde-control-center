cmake_minimum_required(VERSION 3.18)

set(PLUGIN_NAME dock)

file(GLOB_RECURSE Dock_Plugin_SRCS
    "operation/*.cpp"
    "operation/*.hpp"
    "operation/*.h"
    "res/dcc-dock-plugin.qrc" 
)

add_library(${PLUGIN_NAME} MODULE
    ${Dock_Plugin_SRCS}
)

target_link_libraries(${PLUGIN_NAME} PRIVATE
    ${DCC_FRAME_Library}
    ${DTK_NS}::Gui
)

dcc_install_plugin(NAME ${PLUGIN_NAME} TARGET ${PLUGIN_NAME})
