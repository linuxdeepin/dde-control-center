cmake_minimum_required(VERSION 3.18)
set(pluginName deepinid)
file(GLOB_RECURSE deepinid_SRCS
    "operation/*.cpp"
    "operation/*.h"
    "operation/qrc/deepinid.qrc"
)

find_package(OpenSSL REQUIRED)

add_library(${pluginName} MODULE
    ${deepinid_SRCS}
)

set(deepinid_Libraries
    ${DCC_FRAME_Library}
    ${QT_NS}::DBus
    ${QT_NS}::Concurrent
    ${DTK_NS}::Gui
)

target_link_libraries(${pluginName} PRIVATE
    ${deepinid_Libraries}
    libcrypto.so
)

dcc_install_plugin(NAME ${pluginName} TARGET ${pluginName})
