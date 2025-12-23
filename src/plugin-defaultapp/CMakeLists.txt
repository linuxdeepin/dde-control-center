cmake_minimum_required(VERSION 3.18)
set(DefaultApp_Name defaultapp)
file(GLOB_RECURSE defaultapp_SRCS
    "operation/*.cpp"
)

# List of QML files with their source paths
set(QML_SOURCE_FILES
    qml/defaultappMain.qml
    qml/defaultapp.qml
    qml/DetailItem.qml
)

# Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
foreach(_qml_file IN LISTS QML_SOURCE_FILES)
    get_filename_component(_qml_filename "${_qml_file}" NAME)
    set_source_files_properties("${_qml_file}" PROPERTIES
        QT_RESOURCE_ALIAS "${_qml_filename}"
    )
endforeach()

qt_add_qml_module(${DefaultApp_Name}
    URI "org.deepin.dcc.defaultapp"
    QML_FILES ${QML_SOURCE_FILES}
    SOURCES ${defaultapp_SRCS}
    NO_PLUGIN
)

set(DefaultApp_Libraries
    ${DCC_FRAME_Library}
    ${QT_NS}::DBus
)

target_include_directories(${DefaultApp_Name} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/operation
)

target_link_libraries(${DefaultApp_Name} PRIVATE
    ${DefaultApp_Libraries}
)

dcc_install_plugin(NAME ${DefaultApp_Name} TARGET ${DefaultApp_Name} DISABLE_QML_COPY)
