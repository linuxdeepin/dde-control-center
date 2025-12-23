cmake_minimum_required(VERSION 3.18)
set(pluginName deepinid)
file(GLOB_RECURSE deepinid_SRCS
    "operation/*.cpp"
    "operation/*.h"
    "operation/qrc/deepinid.qrc"
)

# List of QML files with their source paths
set(QML_SOURCE_FILES
    qml/ConFirmDialog.qml
    qml/ConfirmManager.qml
    qml/DeepinIDAccountSecurity.qml
    qml/DeepinIDLogin.qml
    qml/deepinidMain.qml
    qml/deepinid.qml
    qml/DeepinIDSyncService.qml
    qml/DeepinIDUserInfo.qml
    qml/RegisterDialog.qml
    qml/VerifyDialog.qml
)

# Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
foreach(_qml_file IN LISTS QML_SOURCE_FILES)
    get_filename_component(_qml_filename "${_qml_file}" NAME)
    set_source_files_properties("${_qml_file}" PROPERTIES
        QT_RESOURCE_ALIAS "${_qml_filename}"
    )
endforeach()

find_package(OpenSSL REQUIRED)

qt_add_qml_module(${pluginName}
    URI "org.deepin.dcc.deepinid"
    QML_FILES ${QML_SOURCE_FILES}
    SOURCES ${deepinid_SRCS}
    NO_PLUGIN
)

set(deepinid_Libraries
    ${DCC_FRAME_Library}
    ${QT_NS}::DBus
    ${QT_NS}::Concurrent
    ${DTK_NS}::Gui
)

target_include_directories(${pluginName} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/operation
)

target_link_libraries(${pluginName} PRIVATE
    ${deepinid_Libraries}
    libcrypto.so
)

dcc_install_plugin(NAME ${pluginName} TARGET ${pluginName} DISABLE_QML_COPY)
