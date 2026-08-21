cmake_minimum_required(VERSION 3.18)
set(DefaultApp_Name defaultapp)
file(GLOB_RECURSE defaultapp_SRCS
    "operation/*.cpp"
)

add_library(${DefaultApp_Name} MODULE
    ${defaultapp_SRCS}
)

set(DefaultApp_Libraries
    ${DCC_FRAME_Library}
    ${QT_NS}::DBus
)

target_link_libraries(${DefaultApp_Name} PRIVATE
    ${DefaultApp_Libraries}
)

dcc_install_plugin(NAME ${DefaultApp_Name} TARGET ${DefaultApp_Name})
