cmake_minimum_required(VERSION 3.18)
set(Power_Name power)
file(GLOB_RECURSE power_SRCS
    "operation/*.cpp"
    "operation/*.h"
    "operation/qrc/power.qrc"
)

# List of QML files with their source paths
set(QML_SOURCE_FILES
    qml/BatteryPage.qml
    qml/CustomComboBox.qml
    qml/CustomTipsSlider.qml
    qml/GeneralPage.qml
    qml/powerMain.qml
    qml/PowerPage.qml
    qml/PowerPlansListview.qml
    qml/power.qml
    qml/ScheduledShutdownDialog.qml
)

# Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
foreach(_qml_file IN LISTS QML_SOURCE_FILES)
    get_filename_component(_qml_filename "${_qml_file}" NAME)
    set_source_files_properties("${_qml_file}" PROPERTIES
        QT_RESOURCE_ALIAS "${_qml_filename}"
    )
endforeach()

qt_add_qml_module(${Power_Name}
    URI "org.deepin.dcc.power"
    QML_FILES ${QML_SOURCE_FILES}
    SOURCES ${power_SRCS}
    NO_PLUGIN
)

set(Power_Libraries
    ${DCC_FRAME_Library}
    ${DTK_NS}::Gui
    ${QT_NS}::DBus
    ${QT_NS}::Concurrent
)

target_include_directories(${Power_Name} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/operation
)

target_link_libraries(${Power_Name} PRIVATE
    ${Power_Libraries}
)

dcc_install_plugin(NAME ${Power_Name} TARGET ${Power_Name} DISABLE_QML_COPY)
