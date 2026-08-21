cmake_minimum_required(VERSION 3.18)
set(pluginName accounts)
file(GLOB_RECURSE accounts_SRCS
    "operation/*.cpp"
    "operation/*.h"
    "operation/qrc/${pluginName}.qrc"
)

add_library(${pluginName} MODULE
    ${accounts_SRCS}
)

pkg_check_modules(DEEPIN_PW_CHECK REQUIRED libdeepin_pw_check)

find_package(PolkitQt6-1)
set(accounts_Libraries
    ${DCC_FRAME_Library}
    ${QT_NS}::DBus
    ${DTK_NS}::Core
    ${DTK_NS}::Gui
    ${QT_NS}::Concurrent
    PolkitQt6-1::Agent
    ${DEEPIN_PW_CHECK_LIBRARIES}
)

target_link_libraries(${pluginName} PRIVATE
    ${accounts_Libraries}
)

dcc_install_plugin(NAME ${pluginName} TARGET ${pluginName})
