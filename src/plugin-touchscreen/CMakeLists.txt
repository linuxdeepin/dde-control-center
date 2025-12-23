if (BUILD_PLUGIN)
    set(touchscreen_Name touchscreen)
    file(GLOB_RECURSE touchscreen_SRCS
        "operation/*.cpp"
        "operation/qrc/touchscreen.qrc"
    )
    file(GLOB_RECURSE touchscreen_qml_SRCS
        "qml/*.qml"
    )
    
    # List of QML files with their source paths
    set(QML_SOURCE_FILES
        qml/touchscreenMain.qml
        qml/touchscreen.qml
        qml/TouchScreen.qml
    )

    # Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
    foreach(_qml_file IN LISTS QML_SOURCE_FILES)
        get_filename_component(_qml_filename "${_qml_file}" NAME)
        set_source_files_properties("${_qml_file}" PROPERTIES
            QT_RESOURCE_ALIAS "${_qml_filename}"
        )
    endforeach()
    
    # pkg_check_modules(QGSettings REQUIRED IMPORTED_TARGET gsettings-qt)
    qt_add_qml_module(${touchscreen_Name}
        URI "org.deepin.dcc.touchscreen"
        QML_FILES ${QML_SOURCE_FILES}
        SOURCES ${touchscreen_SRCS}
        NO_PLUGIN
    )

    set(touchscreen_Includes
        src/plugin-touchscreen/operation
    )
    set(touchscreen_Libraries
        ${DCC_FRAME_Library}
        ${DTK_NS}::Gui
        ${QT_NS}::DBus
        ${QT_NS}::Qml
    )
    target_include_directories(${touchscreen_Name} PUBLIC
        ${touchscreen_Includes}
    )
    target_link_libraries(${touchscreen_Name} PRIVATE
        ${touchscreen_Libraries}
    )
    dcc_install_plugin(NAME ${touchscreen_Name} TARGET ${touchscreen_Name} DISABLE_QML_COPY)
    # dcc_handle_plugin_translation(NAME ${touchscreen_Name}  QML_FILES ${touchscreen_qml_SRCS} SOURCE_FILES ${touchscreen_SRCS})
endif()
