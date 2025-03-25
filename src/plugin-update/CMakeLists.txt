cmake_minimum_required(VERSION 3.18)
set(Update_Name update)
file(GLOB_RECURSE update_SRCS
        "operation/*.cpp"
        "operation/*.h"
        "operation/qrc/update.qrc"
)

add_library(${Update_Name} MODULE
        ${update_SRCS}
    )

set(Update_Libraries
        ${DCC_FRAME_Library}
        ${QT_NS}::DBus
        ${DTK_NS}::Gui
        ${QT_NS}::Concurrent
)

target_link_libraries(${Update_Name} PRIVATE
        ${Update_Libraries}
)

dcc_install_plugin(NAME ${Update_Name} TARGET ${Update_Name})
