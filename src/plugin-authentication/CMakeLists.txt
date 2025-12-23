cmake_minimum_required(VERSION 3.18)
set(pluginName authentication)
file(GLOB_RECURSE authentication_SRCS
    "operation/*.cpp"
    "operation/*.h"
    "operation/qrc/${pluginName}.qrc"
)

# List of QML files with their source paths
set(QML_SOURCE_FILES
    qml/AddFaceinfoDialog.qml
    qml/AddFingerDialog.qml
    qml/AddIrisDialog.qml
    qml/authenticationMain.qml
    qml/authentication.qml
    qml/DisclaimerControl.qml
)

# Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
foreach(_qml_file IN LISTS QML_SOURCE_FILES)
    get_filename_component(_qml_filename "${_qml_file}" NAME)
    set_source_files_properties("${_qml_file}" PROPERTIES
        QT_RESOURCE_ALIAS "${_qml_filename}"
    )
endforeach()

qt_add_qml_module(${pluginName}
    URI "org.deepin.dcc.authentication"
    QML_FILES ${QML_SOURCE_FILES}
    SOURCES ${authentication_SRCS}
    NO_PLUGIN
)

pkg_check_modules(DEEPIN_PW_CHECK REQUIRED libdeepin_pw_check)
pkg_check_modules(DaReader REQUIRED dareader)

find_package(PolkitQt6-1)
set(authentication_Libraries
    ${DCC_FRAME_Library}
    ${QT_NS}::DBus
    ${DTK_NS}::Core
    ${DTK_NS}::Gui
    ${QT_NS}::Concurrent
    PolkitQt6-1::Agent
    ${DEEPIN_PW_CHECK_LIBRARIES}
    ${DaReader_LIBRARIES}
)

target_include_directories(${pluginName} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/operation
)

target_link_libraries(${pluginName} PRIVATE
    ${authentication_Libraries}
)

dcc_install_plugin(NAME ${pluginName} TARGET ${pluginName} DISABLE_QML_COPY)
