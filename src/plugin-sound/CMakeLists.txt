cmake_minimum_required(VERSION 3.18)
set(Sound_Name sound)
file(GLOB_RECURSE Sound_SRCS
    "operation/*.cpp"
    "operation/*.h"
    "operation/qrc/sound.qrc"
)

# List of QML files with their source paths
set(QML_SOURCE_FILES
    qml/DeviceListView.qml
    qml/MicrophonePage.qml
    qml/SoundDevicemanagesPage.qml
    qml/SoundEffectsPage.qml
    qml/soundMain.qml
    qml/sound.qml
    qml/SpeakerPage.qml
)

# Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
foreach(_qml_file IN LISTS QML_SOURCE_FILES)
    get_filename_component(_qml_filename "${_qml_file}" NAME)
    set_source_files_properties("${_qml_file}" PROPERTIES
        QT_RESOURCE_ALIAS "${_qml_filename}"
    )
endforeach()

qt_add_qml_module(${Sound_Name}
    URI "org.deepin.dcc.sound"
    QML_FILES ${QML_SOURCE_FILES}
    SOURCES ${Sound_SRCS}
    NO_PLUGIN
)

if (DISABLE_SOUND_ADVANCED)
    target_compile_definitions(${Sound_Name} PUBLIC -DDCC_DISABLE_SOUND_ADVANCED)
endif()

set(Sound_Libraries
    ${DCC_FRAME_Library}
    ${DTK_NS}::Gui
    ${QT_NS}::DBus
    ${QT_NS}::Multimedia
)

target_link_libraries(${Sound_Name} PRIVATE
    ${Sound_Libraries}
)

target_include_directories(${Sound_Name} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/operation
)

dcc_install_plugin(NAME ${Sound_Name} TARGET ${Sound_Name} DISABLE_QML_COPY)
# QML_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/plugin-sound/qml)
