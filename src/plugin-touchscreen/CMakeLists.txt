if (BUILD_PLUGIN)
    set(touchscreen_Name touchscreen)
    file(GLOB_RECURSE touchscreen_SRCS
        "operation/*.cpp"
        "operation/qrc/touchscreen.qrc"
    )
    file(GLOB_RECURSE touchscreen_qml_SRCS
        "qml/*.qml"
    )
    find_package(${QT_NS} COMPONENTS Concurrent REQUIRED COMPONENTS Qml)
    # pkg_check_modules(QGSettings REQUIRED IMPORTED_TARGET gsettings-qt)
    add_library(${touchscreen_Name} MODULE
        ${touchscreen_SRCS}
    )

    set(touchscreen_Includes
        src/plugin-touchscreen/operation
        ../../include/interface
    )
    set(touchscreen_Libraries
        ${DTK_NS}::Gui
        ${QT_NS}::DBus
        ${QT_NS}::Qml
        ${Interface_Name}
        Qt::Concurrent
    )
    target_include_directories(${touchscreen_Name} PUBLIC
        ${touchscreen_Includes}
    )
    target_link_libraries(${touchscreen_Name} PRIVATE
        ${touchscreen_Libraries}
    )
    dcc_install_plugin(NAME ${touchscreen_Name} TARGET ${touchscreen_Name})
    # dcc_handle_plugin_translation(NAME ${touchscreen_Name}  QML_FILES ${touchscreen_qml_SRCS} SOURCE_FILES ${touchscreen_SRCS})
endif()
