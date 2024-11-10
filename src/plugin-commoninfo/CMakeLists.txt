cmake_minimum_required(VERSION 3.18)
set(CommonInfo_Name commonInfo)

pkg_check_modules(DEEPIN_PW_CHECK REQUIRED libdeepin_pw_check)


file(GLOB_RECURSE commonInfo_SRCS
    "operation/*.cpp"
    "operation/*.h"
    "operation/qrc/commoninfo.qrc"
)

add_library(${CommonInfo_Name} MODULE
    ${commonInfo_SRCS}
    operation/grubmenulistmodel.h
    operation/grubmenulistmodel.cpp
)

set(CommonInfo_Libraries
    ${DCC_FRAME_Library}
    ${QT_NS}::DBus
    ${DTK_NS}::Gui
)

target_link_libraries(${CommonInfo_Name} PRIVATE
    ${CommonInfo_Libraries}
    ${DEEPIN_PW_CHECK_LIBRARIES}
)

dcc_install_plugin(NAME ${CommonInfo_Name} TARGET ${CommonInfo_Name})
