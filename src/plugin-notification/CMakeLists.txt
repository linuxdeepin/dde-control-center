set(QT_VERSION_MAJOR "6")
find_package(Dtk6 REQUIRED COMPONENTS Tools) # for ${DTK_XML2CPP} which provides the path of `qdbusxml2cpp-fix` binary

if (BUILD_PLUGIN)

    set(notification_Name notification)
    file(GLOB_RECURSE notification_SRCS
        "operation/*.cpp"
        "operation/*.h"
        "types/*.h"
        "operation/qrc/notification.qrc"
    )
    file(GLOB_RECURSE notification_qml_SRCS
        "qml/*.qml"
    )
    find_package(${QT_NS} COMPONENTS Concurrent REQUIRED COMPONENTS Qml)

    set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/operation/xml/org.desktopspec.ApplicationManager1.Application.xml
        PROPERTIES  INCLUDE types/am.h
                    CLASSNAME AppManager1Application
    )

    set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/operation/xml/org.desktopspec.ObjectManager1.xml
        PROPERTIES  INCLUDE types/am.h
                    CLASSNAME AppManager1ApplicationObjectManager
    )

    qt_add_dbus_interfaces(
        DBUS_INTERFACES
        ${CMAKE_CURRENT_SOURCE_DIR}/operation/xml/org.desktopspec.ApplicationManager1.Application.xml
    )

    qt_add_dbus_interfaces(
        DBUS_INTERFACES
        ${CMAKE_CURRENT_SOURCE_DIR}/operation/xml/org.desktopspec.ObjectManager1.xml
    )

    # List of QML files with their source paths
    set(QML_SOURCE_FILES
        qml/ImageCheckBox.qml
        qml/notificationMain.qml
        qml/notification.qml
        qml/TimeRange.qml
    )

    # Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
    foreach(_qml_file IN LISTS QML_SOURCE_FILES)
        get_filename_component(_qml_filename "${_qml_file}" NAME)
        set_source_files_properties("${_qml_file}" PROPERTIES
            QT_RESOURCE_ALIAS "${_qml_filename}"
        )
    endforeach()

    qt_add_qml_module(${notification_Name}
        URI "org.deepin.dcc.notification"
        QML_FILES ${QML_SOURCE_FILES}
        SOURCES ${notification_SRCS}
            ${DBUS_INTERFACES}
        NO_PLUGIN
    )

    set(notification_Includes
        src/plugin-notification/operation
        src/plugin-notification
        ${PROJECT_BINARY_DIR}/src/plugin-notification
    )
    set(notification_Libraries
        ${DCC_FRAME_Library}
        ${DTK_NS}::Gui
        ${QT_NS}::DBus
        ${QT_NS}::Concurrent
    )
    target_include_directories(${notification_Name} PUBLIC
        ${notification_Includes}
    )
    target_link_libraries(${notification_Name} PRIVATE
        ${notification_Libraries}
    )

    dcc_install_plugin(NAME ${notification_Name} TARGET ${notification_Name} DISABLE_QML_COPY)
endif()
