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

set(personalization_Includes
    src/plugin-personalization/operation
)

add_library(${LIB_NAME} MODULE
    ${Personalization_SRCS}
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

target_include_directories(${LIB_NAME} PUBLIC
    ${personalization_Includes}
)

target_link_libraries(${LIB_NAME} PRIVATE
    ${Personalization_Libraries}
)

dcc_install_plugin(NAME ${LIB_NAME} TARGET ${LIB_NAME})
