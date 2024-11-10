if (BUILD_PLUGIN)
    set(Mouse_Name mouse)
    file(GLOB_RECURSE mouse_SRCS
        "operation/*.cpp"
        "operation/qrc/mouse.qrc"
    )
    file(GLOB_RECURSE mouse_qml_SRCS
        "qml/*.qml"
    )
    # pkg_check_modules(QGSettings REQUIRED IMPORTED_TARGET gsettings-qt)
    add_library(${Mouse_Name} MODULE
        ${mouse_SRCS}
    )

    set(mouse_Includes
        src/plugin-mouse/operation
    )
    set(mouse_Libraries
        ${DCC_FRAME_Library}
        ${DTK_NS}::Gui
        ${QT_NS}::DBus
        ${QT_NS}::Qml
        # PkgConfig::QGSettings
    )
    target_include_directories(${Mouse_Name} PUBLIC
        ${mouse_Includes}
    )
    target_link_libraries(${Mouse_Name} PRIVATE
        ${mouse_Libraries}
    )
    dcc_install_plugin(NAME ${Mouse_Name} TARGET ${Mouse_Name})
    # dcc_handle_plugin_translation(NAME ${Mouse_Name}  QML_FILES ${mouse_qml_SRCS} SOURCE_FILES ${mouse_SRCS})
endif()
