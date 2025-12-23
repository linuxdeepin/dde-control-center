cmake_minimum_required(VERSION 3.18)

set(PLUGIN_NAME dock)

file(GLOB_RECURSE Dock_Plugin_SRCS
    "operation/*.cpp"
    "operation/*.hpp"
    "operation/*.h"
    "res/dcc-dock-plugin.qrc" 
)

# List of QML files with their source paths
set(QML_SOURCE_FILES
    qml/CustomComBobox.qml
    qml/dock.qml
    qml/main.qml
    qml/PluginArea.qml
)

# Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
foreach(_qml_file IN LISTS QML_SOURCE_FILES)
    get_filename_component(_qml_filename "${_qml_file}" NAME)
    set_source_files_properties("${_qml_file}" PROPERTIES
        QT_RESOURCE_ALIAS "${_qml_filename}"
    )
endforeach()

qt_add_qml_module(${PLUGIN_NAME}
    URI "org.deepin.dcc.dock"
    QML_FILES ${QML_SOURCE_FILES}
    SOURCES ${Dock_Plugin_SRCS}
    NO_PLUGIN
)

target_include_directories(${PLUGIN_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/operation
)

target_link_libraries(${PLUGIN_NAME} PRIVATE
    ${DCC_FRAME_Library}
    ${DTK_NS}::Gui
)

dcc_install_plugin(NAME ${PLUGIN_NAME} TARGET ${PLUGIN_NAME} DISABLE_QML_COPY)
