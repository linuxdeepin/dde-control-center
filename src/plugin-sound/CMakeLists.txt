cmake_minimum_required(VERSION 3.18)
set(Sound_Name sound)
file(GLOB_RECURSE Sound_SRCS
    "operation/*.cpp"
    "operation/qrc/sound.qrc"
)

add_library(${Sound_Name} MODULE
    ${Sound_SRCS}
    operation/soundInteraction.cpp
    operation/soundInteraction.h
    operation/soundDeviceModel.cpp
    operation/soundDeviceModel.h
    operation/soundDeviceData.cpp
    operation/soundDeviceData.h
    operation/port.cpp
    operation/port.h
    operation/audioservermodel.cpp
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

dcc_install_plugin(NAME ${Sound_Name} TARGET ${Sound_Name})
# QML_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/plugin-sound/qml)
