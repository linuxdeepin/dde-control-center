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

    add_library(${notification_Name} MODULE
        ${notification_SRCS}
        ${DBUS_INTERFACES}
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

    dcc_install_plugin(NAME ${notification_Name} TARGET ${notification_Name})
endif()
