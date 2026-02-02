cmake_minimum_required(VERSION 3.23) # qt_add_qml_module函数支持最小版本3.18,建议版本3.23+
# 该name会设置为插件名，只支持字母加数字，需要与{Name}.qml中DccObject的name相同，用于插件禁用操作
set(PLUGIN_NAME "example")

find_package(Qt6 COMPONENTS Core LinguistTools REQUIRED) # dcc_handle_plugin_translation中用到LinguistTools的函数
find_package(DdeControlCenter REQUIRED)

file(GLOB_RECURSE PLUGIN_SRCS
        "src/*.cpp"
        "src/*.h"
        # "src/qrc/example.qrc"
)
add_library(${PLUGIN_NAME} MODULE
        ${PLUGIN_SRCS}
)

# target_include_directories(${PLUGIN_NAME} PUBLIC
#     Dde::Control-Center
# )

target_link_libraries(${PLUGIN_NAME} PRIVATE
    Dde::Control-Center # 添加dde-control-center库
    Qt6::Core
)
# 处理插件安装
dcc_install_plugin(NAME ${PLUGIN_NAME} TARGET ${PLUGIN_NAME})
# 处理翻译和安装，如果自己处理翻译，可以不调用该函数
dcc_handle_plugin_translation(NAME ${PLUGIN_NAME} )
