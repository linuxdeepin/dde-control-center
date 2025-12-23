cmake_minimum_required(VERSION 3.18)
set(SystemInfo_Name systemInfo)
file(GLOB_RECURSE systemInfo_SRCS
    "operation/*.cpp"
    "operation/*.h"
    "operation/qrc/systeminfo.qrc"
)

# List of QML files with their source paths
set(QML_SOURCE_FILES
    qml/NativeInfoPage.qml
    qml/PrivacyPolicyPage.qml
    qml/systemInfoMain.qml
    qml/systemInfo.qml
    qml/UserExperienceProgramPage.qml
    qml/UserLicensePage.qml
    qml/VersionProtocolPage.qml
)

# Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
foreach(_qml_file IN LISTS QML_SOURCE_FILES)
    get_filename_component(_qml_filename "${_qml_file}" NAME)
    set_source_files_properties("${_qml_file}" PROPERTIES
        QT_RESOURCE_ALIAS "${_qml_filename}"
    )
endforeach()

qt_add_qml_module(${SystemInfo_Name}
    URI "org.deepin.dcc.systemInfo"
    QML_FILES ${QML_SOURCE_FILES}
    SOURCES ${systemInfo_SRCS}
        operation/utils.h
    NO_PLUGIN
)

set(SystemInfo_Libraries
    ${DCC_FRAME_Library}
    ${QT_NS}::DBus
    ${DTK_NS}::Gui
)

target_include_directories(${SystemInfo_Name} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/operation
)

target_link_libraries(${SystemInfo_Name} PRIVATE
    ${SystemInfo_Libraries}
)

dcc_install_plugin(NAME ${SystemInfo_Name} TARGET ${SystemInfo_Name} DISABLE_QML_COPY)
