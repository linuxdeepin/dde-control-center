if (BUILD_PLUGIN)
    set(wacom_Name wacom)
    file(GLOB_RECURSE wacom_SRCS
        "operation/*.cpp"
        "operation/qrc/wacom.qrc"
    )
    file(GLOB_RECURSE wacom_qml_SRCS
        "qml/*.qml"
    )
    find_package(${QT_NS} COMPONENTS Concurrent REQUIRED COMPONENTS Qml)
    add_library(${wacom_Name} MODULE
        ${wacom_SRCS}
    )

    set(wacom_Includes
        src/plugin-wacom/operation
    )
    set(wacom_Libraries
        ${DCC_FRAME_Library}
        ${DTK_NS}::Gui
        ${QT_NS}::DBus
    )
    target_include_directories(${wacom_Name} PUBLIC
        ${wacom_Includes}
    )
    target_link_libraries(${wacom_Name} PRIVATE
        ${wacom_Libraries}
    )
    dcc_install_plugin(NAME ${wacom_Name} TARGET ${wacom_Name})
endif()
