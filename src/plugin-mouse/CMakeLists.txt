if (BUILD_PLUGIN)

    set(Mouse_Name mouse)
    find_package(Qt6 REQUIRED COMPONENTS WaylandClient)
    if(Qt6_VERSION VERSION_GREATER_EQUAL 6.10)
        find_package(Qt6 COMPONENTS WaylandClientPrivate REQUIRED)
    endif()

    if (Enable_TreelandSupport)
    find_package(TreelandProtocols REQUIRED)
    endif()

    file(GLOB_RECURSE mouse_SRCS
        "operation/*.cpp"
        "operation/qrc/mouse.qrc"
    )
    file(GLOB_RECURSE mouse_qml_SRCS
        "qml/*.qml"
    )
    
    # List of QML files with their source paths
    set(QML_SOURCE_FILES
        qml/ClickTest.qml
        qml/Common.qml
        qml/GestureGroup.qml
        qml/mouseMain.qml
        qml/mouse.qml
        qml/Mouse.qml
        qml/Touchpad.qml
    )

    # Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
    foreach(_qml_file IN LISTS QML_SOURCE_FILES)
        get_filename_component(_qml_filename "${_qml_file}" NAME)
        set_source_files_properties("${_qml_file}" PROPERTIES
            QT_RESOURCE_ALIAS "${_qml_filename}"
        )
    endforeach()
    
    # pkg_check_modules(QGSettings REQUIRED IMPORTED_TARGET gsettings-qt)
    qt_add_qml_module(${Mouse_Name}
        URI "org.deepin.dcc.mouse"
        QML_FILES ${QML_SOURCE_FILES}
        SOURCES ${mouse_SRCS}
        NO_PLUGIN
    )

    if (Enable_TreelandSupport)
    qt6_generate_wayland_protocol_client_sources(${Mouse_Name} FILES
        ${TREELAND_PROTOCOLS_DATA_DIR}/treeland-personalization-manager-v1.xml
    )

    target_compile_definitions(${Mouse_Name} PRIVATE Enable_Treeland)
    endif()

    set(mouse_Includes
        src/plugin-mouse/operation
    )
    set(mouse_Libraries
        ${DCC_FRAME_Library}
        ${DTK_NS}::Gui
        ${QT_NS}::DBus
        ${QT_NS}::Qml
        ${QT_NS}::WaylandClientPrivate
        # PkgConfig::QGSettings
    )
    target_include_directories(${Mouse_Name} PUBLIC
        ${mouse_Includes}
    )
    target_link_libraries(${Mouse_Name} PRIVATE
        ${mouse_Libraries}
    )
    dcc_install_plugin(NAME ${Mouse_Name} TARGET ${Mouse_Name} DISABLE_QML_COPY)
    # dcc_handle_plugin_translation(NAME ${Mouse_Name}  QML_FILES ${mouse_qml_SRCS} SOURCE_FILES ${mouse_SRCS})
endif()
