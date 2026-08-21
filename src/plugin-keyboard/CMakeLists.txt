if (BUILD_PLUGIN)
    set(Plugin_Name keyboard)
    file(GLOB_RECURSE keyboard_SRCS
        "operation/*.h"
        "operation/*.cpp"
        "operation/qrc/keyboard.qrc"
    )
    if (NOT Enable_TreelandSupport)
        list(FILTER keyboard_SRCS EXCLUDE REGEX ".*/operation/keyboardwaylandproxy\\.(h|cpp)$")
    endif()
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
    find_package(PolkitQt6-1 REQUIRED)
    find_package(${QT_NS} COMPONENTS Quick Gui WaylandClient REQUIRED)
    find_package(PkgConfig REQUIRED)
    find_package(TreelandProtocols REQUIRED)
    pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)

    qt6_generate_wayland_protocol_client_sources(${Plugin_Name}
        FILES
            ${TREELAND_PROTOCOLS_DATA_DIR}/treeland-shortcut-manager-v2.xml
    )

    set(keyboard_Libraries
        ${DCC_FRAME_Library}
        ${DTK_NS}::Gui
        ${QT_NS}::DBus
        ${QT_NS}::Gui
        ${QT_NS}::Qml
        ${QT_NS}::Quick
        ${QT_NS}::WaylandClient
        PolkitQt6-1::Agent
        ${WAYLAND_CLIENT_LIBRARIES}
    )
    target_include_directories(${Plugin_Name} PUBLIC
        ${keyboard_Includes}
        ${WAYLAND_CLIENT_INCLUDE_DIRS}
        ${Qt6Gui_PRIVATE_INCLUDE_DIRS}
    )
    target_link_libraries(${Plugin_Name} PRIVATE
        ${keyboard_Libraries}
        dcc-shared-utils
    )
    dcc_install_plugin(NAME ${Plugin_Name} TARGET ${Plugin_Name})
    # dcc_handle_plugin_translation(NAME ${Plugin_Name}  QML_FILES ${keyboard_qml_SRCS} SOURCE_FILES ${keyboard_SRCS})
endif()
