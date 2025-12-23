cmake_minimum_required(VERSION 3.18)
set(LIB_NAME personalization)

find_package(Qt6 REQUIRED COMPONENTS WaylandClient)
if(Qt6_VERSION VERSION_GREATER_EQUAL 6.10)
  find_package(Qt6 COMPONENTS WaylandClientPrivate REQUIRED)
endif()

if (Enable_TreelandSupport)
find_package(TreelandProtocols REQUIRED)
endif()

file(GLOB_RECURSE Personalization_SRCS
    "operation/*.cpp"
    "operation/*.hpp"
    "operation/*.h"
    "operation/qrc/personalization.qrc"
    )

# List of QML files with their source paths
set(QML_SOURCE_FILES
    qml/ColorAndIcons.qml
    qml/CustomComboBox.qml
    qml/DccColorDialog.qml
    qml/DccSaturationLightnessPicker.qml
    qml/FontCombobox.qml
    qml/FontSizePage.qml
    qml/IconThemeGridView.qml
    qml/InterfaceEffectListview.qml
    qml/personalizationMain.qml
    qml/personalization.qml
    qml/ScreenIndicator.qml
    qml/ScreenSaverPage.qml
    qml/ScreenTab.qml
    qml/ThemeSelectView.qml
    qml/WallpaperPage.qml
    qml/WallpaperSelectView.qml
    qml/WindowEffectPage.qml
)

# Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
foreach(_qml_file IN LISTS QML_SOURCE_FILES)
    get_filename_component(_qml_filename "${_qml_file}" NAME)
    set_source_files_properties("${_qml_file}" PROPERTIES
        QT_RESOURCE_ALIAS "${_qml_filename}"
    )
endforeach()

qt_add_qml_module(${LIB_NAME}
    URI "org.deepin.dcc.personalization"
    QML_FILES ${QML_SOURCE_FILES}
    SOURCES ${Personalization_SRCS}
    NO_PLUGIN
)

if (Enable_TreelandSupport)
qt6_generate_wayland_protocol_client_sources(${LIB_NAME} FILES
    ${TREELAND_PROTOCOLS_DATA_DIR}/treeland-personalization-manager-v1.xml
)

target_compile_definitions(${LIB_NAME} PRIVATE Enable_Treeland)
endif()


set(Personalization_Libraries
    ${DCC_FRAME_Library}
    ${DTK_NS}::Gui
    ${QT_NS}::DBus
    ${QT_NS}::Qml
    ${QT_NS}::WaylandClientPrivate
)

target_include_directories(${LIB_NAME} PRIVATE
    operation
)

target_link_libraries(${LIB_NAME} PRIVATE
    ${Personalization_Libraries}
)

dcc_install_plugin(NAME ${LIB_NAME} TARGET ${LIB_NAME} DISABLE_QML_COPY)
