set(LIB_NAME privacy)

find_package(DDEShell REQUIRED)
find_package(PolkitQt6-1 REQUIRED)

file(GLOB_RECURSE Privacy_SRCS
    "operation/*.cpp"
    "operation/*.hpp"
    "operation/*.h"
    "operation/qrc/privacy.qrc"
)

set(Privacy_Includes
    src/plugin-privacy/operation
)

add_library(${LIB_NAME} MODULE
    ${Privacy_SRCS}
    ${DBUS_INTERFACES}
)

set(Privacy_Libraries
    ${DCC_FRAME_Library}
    ${DTK_NS}::Gui
    ${QT_NS}::DBus
    ${QT_NS}::Qml
    ${QT_NS}::Concurrent
    Dde::Shell
    PolkitQt6-1::Agent
)

target_include_directories(${LIB_NAME} PUBLIC
    ${Privacy_Includes}
)

target_link_libraries(${LIB_NAME} PRIVATE
    ${Privacy_Libraries}
)

dcc_install_plugin(NAME ${LIB_NAME} TARGET ${LIB_NAME})
