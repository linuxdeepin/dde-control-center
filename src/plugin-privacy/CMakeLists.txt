set(LIB_NAME privacy)

find_package(DDEShell REQUIRED)
find_package(PolkitQt6-1 REQUIRED)

file(GLOB_RECURSE Privacy_SRCS
    "operation/*.cpp"
    "operation/*.hpp"
    "operation/*.h"
    "operation/qrc/privacy.qrc"
)

# List of QML files with their source paths
set(QML_SOURCE_FILES
    qml/Camera.qml
    qml/FileAndFolder.qml
    qml/privacyMain.qml
    qml/privacy.qml
)

# Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
foreach(_qml_file IN LISTS QML_SOURCE_FILES)
    get_filename_component(_qml_filename "${_qml_file}" NAME)
    set_source_files_properties("${_qml_file}" PROPERTIES
        QT_RESOURCE_ALIAS "${_qml_filename}"
    )
endforeach()

set(Privacy_Includes
    src/plugin-privacy/operation
)

qt_add_qml_module(${LIB_NAME}
    URI "org.deepin.dcc.privacy"
    QML_FILES ${QML_SOURCE_FILES}
    SOURCES ${Privacy_SRCS}
        ${DBUS_INTERFACES}
    NO_PLUGIN
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

dcc_install_plugin(NAME ${LIB_NAME} TARGET ${LIB_NAME} DISABLE_QML_COPY)
