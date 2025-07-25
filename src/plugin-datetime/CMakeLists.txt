cmake_minimum_required(VERSION 3.18)
set(pluginName datetime)
file(GLOB_RECURSE datetime_SRCS
    "operation/*.cpp"
    "operation/*.h"
    "operation/timezoneMap/*.h"
    "operation/timezoneMap/*.cpp"
    "operation/qrc/datetime.qrc"
)

add_library(${pluginName} MODULE
    ${datetime_SRCS}
)

find_package(ICU COMPONENTS i18n uc)

set(dateTime_Libraries
    ${DCC_FRAME_Library}
    ${QT_NS}::DBus
    ${QT_NS}::Concurrent
     ${DTK_NS}::Gui
     ICU::i18n
     ICU::uc
)

target_link_libraries(${pluginName} PRIVATE
    ${dateTime_Libraries}
    dcc-shared-utils
)

dcc_install_plugin(NAME ${pluginName} TARGET ${pluginName})

# keyboard_language_*.ts生成使用脚本misc/translate_language2ts.sh手动执行
