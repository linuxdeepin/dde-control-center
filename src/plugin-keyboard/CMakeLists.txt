if (BUILD_PLUGIN)
    set(Plugin_Name keyboard)
    file(GLOB_RECURSE keyboard_SRCS
        "operation/*.h"
        "operation/*.cpp"
        "operation/qrc/keyboard.qrc"
    )
    file(GLOB_RECURSE keyboard_qml_SRCS
        "qml/*.qml"
    )
    
    # List of QML files with their source paths
    set(QML_SOURCE_FILES
        qml/Common.qml
        qml/keyboardMain.qml
        qml/keyboard.qml
        qml/KeySequenceDisplay.qml
        qml/ShortcutSettingDialog.qml
        qml/Shortcuts.qml
    )

    # Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
    foreach(_qml_file IN LISTS QML_SOURCE_FILES)
        get_filename_component(_qml_filename "${_qml_file}" NAME)
        set_source_files_properties("${_qml_file}" PROPERTIES
            QT_RESOURCE_ALIAS "${_qml_filename}"
        )
    endforeach()
    
    # pkg_check_modules(QGSettings REQUIRED IMPORTED_TARGET gsettings-qt)
    qt_add_qml_module(${Plugin_Name}
        URI "org.deepin.dcc.keyboard"
        QML_FILES ${QML_SOURCE_FILES}
        SOURCES ${keyboard_SRCS}
        NO_PLUGIN
    )

    set(keyboard_Includes
        src/plugin-keyboard/operation
    )
    find_package(PolkitQt6-1 REQUIRED)
    set(keyboard_Libraries
        ${DCC_FRAME_Library}
        ${DTK_NS}::Gui
        ${QT_NS}::DBus
        ${QT_NS}::Gui
        ${QT_NS}::Qml
        PolkitQt6-1::Agent
    )
    target_include_directories(${Plugin_Name} PUBLIC
        ${keyboard_Includes}
    )
    target_link_libraries(${Plugin_Name} PRIVATE
        ${keyboard_Libraries}
        dcc-shared-utils
    )
    dcc_install_plugin(NAME ${Plugin_Name} TARGET ${Plugin_Name} DISABLE_QML_COPY)
    # dcc_handle_plugin_translation(NAME ${Plugin_Name}  QML_FILES ${keyboard_qml_SRCS} SOURCE_FILES ${keyboard_SRCS})
endif()
