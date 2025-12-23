cmake_minimum_required(VERSION 3.18)
set(pluginName accounts)
file(GLOB_RECURSE accounts_SRCS
    "operation/*.cpp"
    "operation/*.h"
    "operation/qrc/${pluginName}.qrc"
)

# List of QML files with their source paths
set(QML_SOURCE_FILES
    qml/AccountSettings.qml
    qml/accountsMain.qml
    qml/accounts.qml
    qml/AutoLoginWarningDialog.qml
    qml/AvatarGridView.qml
    qml/AvatarSettingsDialog.qml
    qml/ComfirmDeleteDialog.qml
    qml/ComfirmSafePage.qml
    qml/CreateAccountDialog.qml
    qml/CustomAvatarCropper.qml
    qml/CustomAvatarEmpatyArea.qml
    qml/CustomLocalAvatarsRow.qml
    qml/EditActionLabel.qml
    qml/LoginMethod.qml
    qml/PasswordLayout.qml
    qml/PasswordModifyDialog.qml
)

# Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
foreach(_qml_file IN LISTS QML_SOURCE_FILES)
    get_filename_component(_qml_filename "${_qml_file}" NAME)
    set_source_files_properties("${_qml_file}" PROPERTIES
        QT_RESOURCE_ALIAS "${_qml_filename}"
    )
endforeach()

qt_add_qml_module(${pluginName}
    URI "org.deepin.dcc.accounts"
    QML_FILES ${QML_SOURCE_FILES}
    SOURCES ${accounts_SRCS}
    NO_PLUGIN
)

pkg_check_modules(DEEPIN_PW_CHECK REQUIRED libdeepin_pw_check)

find_package(PolkitQt6-1)
set(accounts_Libraries
    ${DCC_FRAME_Library}
    ${QT_NS}::DBus
    ${DTK_NS}::Core
    ${DTK_NS}::Gui
    ${QT_NS}::Concurrent
    PolkitQt6-1::Agent
    ${DEEPIN_PW_CHECK_LIBRARIES}
)

target_include_directories(${pluginName} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/operation
)

target_link_libraries(${pluginName} PRIVATE
    ${accounts_Libraries}
    crypt
)

dcc_install_plugin(NAME ${pluginName} TARGET ${pluginName} DISABLE_QML_COPY)
