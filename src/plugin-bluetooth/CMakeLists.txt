cmake_minimum_required(VERSION 3.18)
set(BlueTooth_Name blueTooth)
file(GLOB_RECURSE blueTooth_SRCS
    "operation/*.cpp"
    "operation/*.h"
    "operation/qrc/bluetooth.qrc"
)

# List of QML files with their source paths
set(QML_SOURCE_FILES
    qml/Adapters.qml
    qml/BluetoothCtl.qml
    qml/BlueToothDeviceListView.qml
    qml/blueToothMain.qml
    qml/blueTooth.qml
    qml/MyDevice.qml
    qml/OtherDevice.qml
)

# Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
foreach(_qml_file IN LISTS QML_SOURCE_FILES)
    get_filename_component(_qml_filename "${_qml_file}" NAME)
    set_source_files_properties("${_qml_file}" PROPERTIES
        QT_RESOURCE_ALIAS "${_qml_filename}"
    )
endforeach()

qt_add_qml_module(${BlueTooth_Name}
    URI "org.deepin.dcc.blueTooth"
    QML_FILES ${QML_SOURCE_FILES}
    SOURCES ${blueTooth_SRCS}
    NO_PLUGIN
)

set(BlueTooth_Libraries
    ${DCC_FRAME_Library}
    ${QT_NS}::DBus
    ${DTK_NS}::Gui
)

target_include_directories(${BlueTooth_Name} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/operation
)

target_link_libraries(${BlueTooth_Name} PRIVATE
    ${BlueTooth_Libraries}
)

dcc_install_plugin(NAME ${BlueTooth_Name} TARGET ${BlueTooth_Name} DISABLE_QML_COPY)
