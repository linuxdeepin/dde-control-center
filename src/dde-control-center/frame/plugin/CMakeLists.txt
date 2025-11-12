cmake_minimum_required(VERSION 3.18)

set(DCCQmlPlugin_Name dde-control-center-plugin)
set(URI "org.deepin.dcc")
string(REPLACE "." "/" URI_PATH ${URI})

file(GLOB DCCQmlPlugin_SRCS
    "src/*.h"
    "src/*.cpp"
)

file(GLOB QML_PATHS
    "*.qml"
    "*.js"
)

foreach(FILE_PATH ${QML_PATHS})
    string(REGEX REPLACE ".+/(.+)" "\\1" FILE_NAME ${FILE_PATH})
    list(APPEND DCCQmlPlugin_CONTROLS ${FILE_NAME})
endforeach(FILE_PATH)

qt_add_qml_module(${DCCQmlPlugin_Name}
    PLUGIN_TARGET ${DCCQmlPlugin_Name}
    URI ${URI}
    VERSION "1.0"
    SHARED
    NO_GENERATE_PLUGIN_SOURCE
    NO_PLUGIN_OPTIONAL
    QML_FILES
        ${DCCQmlPlugin_CONTROLS}
    SOURCES
        ${DCCQmlPlugin_SRCS}
    OUTPUT_DIRECTORY
        "${PROJECT_BINARY_DIR}/lib/${URI_PATH}"
)

target_include_directories(${DCCQmlPlugin_Name} PRIVATE
    include
)

set(DCCQmlPlugin_Libraries
    ${QT_NS}::Gui
    ${QT_NS}::Quick
    ${DCC_FRAME_Library}
)

target_link_libraries(${DCCQmlPlugin_Name} PRIVATE
    ${DCCQmlPlugin_Libraries}
)
# set(QML_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/qt${QT_VERSION_MAJOR}/qml" CACHE STRING "Qml plugin install directory")
set(QML_INSTALL_DIR ${DCC_INSTALL_DIR})
install(TARGETS ${DCCQmlPlugin_Name} DESTINATION "${QML_INSTALL_DIR}/${URI_PATH}/")
install(DIRECTORY "${PROJECT_BINARY_DIR}/lib/${URI_PATH}/" DESTINATION "${QML_INSTALL_DIR}/${URI_PATH}/"
    FILES_MATCHING PATTERN "qmldir" PATTERN "*.qmltypes"  PATTERN "*.qml" PATTERN "*.js"
)
