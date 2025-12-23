cmake_minimum_required(VERSION 3.18)
set(CommonInfo_Name commonInfo)

pkg_check_modules(DEEPIN_PW_CHECK REQUIRED libdeepin_pw_check)


file(GLOB_RECURSE commonInfo_SRCS
    "operation/*.cpp"
    "operation/*.h"
    "operation/qrc/commoninfo.qrc"
)

# List of QML files with their source paths
set(QML_SOURCE_FILES
    qml/BootPage.qml
    qml/commonInfoMain.qml
    qml/DevelopModePage.qml
)

# Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
foreach(_qml_file IN LISTS QML_SOURCE_FILES)
    get_filename_component(_qml_filename "${_qml_file}" NAME)
    set_source_files_properties("${_qml_file}" PROPERTIES
        QT_RESOURCE_ALIAS "${_qml_filename}"
    )
endforeach()

qt_add_qml_module(${CommonInfo_Name}
    URI "org.deepin.dcc.commonInfo"
    QML_FILES ${QML_SOURCE_FILES}
    SOURCES ${commonInfo_SRCS}
    NO_PLUGIN
)

set(CommonInfo_Libraries
    ${DCC_FRAME_Library}
    ${QT_NS}::DBus
    ${DTK_NS}::Gui
)

target_include_directories(${CommonInfo_Name} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/operation
)

target_link_libraries(${CommonInfo_Name} PRIVATE
    ${CommonInfo_Libraries}
    ${DEEPIN_PW_CHECK_LIBRARIES}
)

dcc_install_plugin(NAME ${CommonInfo_Name} TARGET ${CommonInfo_Name} DISABLE_QML_COPY)
