if (BUILD_PLUGIN)
    set(wacom_Name wacom)
    file(GLOB_RECURSE wacom_SRCS
        "operation/*.cpp"
        "operation/qrc/wacom.qrc"
    )
    file(GLOB_RECURSE wacom_qml_SRCS
        "qml/*.qml"
    )
    
    # List of QML files with their source paths
    set(QML_SOURCE_FILES
        qml/wacomMain.qml
        qml/wacom.qml
    )

    # Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
    foreach(_qml_file IN LISTS QML_SOURCE_FILES)
        get_filename_component(_qml_filename "${_qml_file}" NAME)
        set_source_files_properties("${_qml_file}" PROPERTIES
            QT_RESOURCE_ALIAS "${_qml_filename}"
        )
    endforeach()
    
    find_package(${QT_NS} COMPONENTS Concurrent REQUIRED COMPONENTS Qml)
    qt_add_qml_module(${wacom_Name}
        URI "org.deepin.dcc.wacom"
        QML_FILES ${QML_SOURCE_FILES}
        SOURCES ${wacom_SRCS}
        NO_PLUGIN
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
    dcc_install_plugin(NAME ${wacom_Name} TARGET ${wacom_Name} DISABLE_QML_COPY)
endif()
