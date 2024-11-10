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
    # pkg_check_modules(QGSettings REQUIRED IMPORTED_TARGET gsettings-qt)
    add_library(${Plugin_Name} MODULE
        ${keyboard_SRCS}
    )

    set(keyboard_Includes
        src/plugin-keyboard/operation
    )
    set(keyboard_Libraries
        ${DCC_FRAME_Library}
        ${DTK_NS}::Gui
        ${QT_NS}::DBus
        ${QT_NS}::Gui
        ${QT_NS}::Qml
    )
    target_include_directories(${Plugin_Name} PUBLIC
        ${keyboard_Includes}
    )
    target_link_libraries(${Plugin_Name} PRIVATE
        ${keyboard_Libraries}
    )
    dcc_install_plugin(NAME ${Plugin_Name} TARGET ${Plugin_Name})
    # dcc_handle_plugin_translation(NAME ${Plugin_Name}  QML_FILES ${keyboard_qml_SRCS} SOURCE_FILES ${keyboard_SRCS})
endif()
