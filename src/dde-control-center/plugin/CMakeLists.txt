cmake_minimum_required(VERSION 3.18)

set(DCCQmlPlugin_Name dde-control-center-lib)

file(GLOB DCCQmlPlugin_SRCS
    "*.h"
    "*.cpp"
)

file(GLOB QML_PATHS
    "*.qml"
    "*.js"
)

foreach(FILE_PATH ${QML_PATHS})
    file(RELATIVE_PATH FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR} ${FILE_PATH})
    list(APPEND DCCQmlPlugin_CONTROLS ${FILE_NAME})
endforeach(FILE_PATH)

file(GLOB RES_PATHS
    "*.dci"
    "*.svg"
    "*.png"
    "*.jpg"
)

foreach(FILE_PATH ${RES_PATHS})
    file(RELATIVE_PATH FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR} ${FILE_PATH})
    list(APPEND DCCQmlPlugin_RES ${FILE_NAME})
endforeach(FILE_PATH)

qt_policy(SET QTP0001 NEW)
qt_add_qml_module(${DCCQmlPlugin_Name}
    PLUGIN_TARGET ${DCCQmlPlugin_Name}
    URI org.deepin.dcc
    VERSION 1.0
    STATIC
    QML_FILES
        ${DCCQmlPlugin_CONTROLS}
    SOURCES
        ${DCCQmlPlugin_SRCS}
    OUTPUT_DIRECTORY
        org/deepin/dcc
)

qt_add_resources(${DCCQmlPlugin_Name} "dci"
    PREFIX "/dsg/built-in-icons"
    FILES ${DCCQmlPlugin_RES}
)

set(DCCQmlPlugin_Libraries
    ${QT_NS}::DBus
    ${QT_NS}::Gui
    ${QT_NS}::Quick
    ${QT_NS}::QuickPrivate
)

target_link_libraries(${DCCQmlPlugin_Name} PRIVATE
    ${DCCQmlPlugin_Libraries}
)
