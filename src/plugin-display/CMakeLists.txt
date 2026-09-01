cmake_minimum_required(VERSION 3.18)

find_package(${QT_NS} REQUIRED COMPONENTS WaylandClient Concurrent)
find_package(PkgConfig REQUIRED)
find_package(TreelandProtocols REQUIRED)
pkg_check_modules(WaylandClient REQUIRED IMPORTED_TARGET wayland-client)
pkg_check_modules(WLR_PROTOCOLS REQUIRED wlr-protocols)

execute_process(
    COMMAND pkg-config --variable=pkgdatadir wlr-protocols
    OUTPUT_VARIABLE WLR_PROTOCOLS_XML_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

file(GLOB_RECURSE WAYLAND_CLIENT_SRCS
    "wayland/client/*.cpp"
    "wayland/client/*.h"
)

add_library(display-wayland-client
    OBJECT
    ${WAYLAND_CLIENT_SRCS}
)

qt_generate_wayland_protocol_client_sources(display-wayland-client
    FILES
        ${WLR_PROTOCOLS_XML_DIR}/unstable/wlr-output-management-unstable-v1.xml
        ${TREELAND_PROTOCOLS_DATA_DIR}/treeland-output-manager-v1.xml
        ${TREELAND_PROTOCOLS_DATA_DIR}/treeland-virtual-output-manager-v1.xml
        ${TREELAND_PROTOCOLS_DATA_DIR}/treeland-wallpaper-manager-unstable-v1.xml
)

# fix arm64 build failed relocation R_AARCH64_ADR_PREL_PG_HI21 against symbol
target_compile_options(display-wayland-client PRIVATE "-fpic")

target_include_directories(display-wayland-client
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/wayland/client"
    PRIVATE
        ${Qt6Gui_PRIVATE_INCLUDE_DIRS}
)

target_link_libraries(display-wayland-client
    PUBLIC
    ${QT_NS}::Core
    ${QT_NS}::Gui
    ${QT_NS}::GuiPrivate
    ${QT_NS}::WaylandClient
    PkgConfig::WaylandClient
)
###########################################
set(Display_Name display)
file(GLOB_RECURSE Display_SRCS
    "operation/*.cpp"
)

add_library(${Display_Name} MODULE
    ${Display_SRCS}
)

target_include_directories(${Display_Name} PUBLIC
    $<TARGET_PROPERTY:display-wayland-client,INTERFACE_INCLUDE_DIRECTORIES>
)

set(Display_Libraries
    ${DCC_FRAME_Library}
    ${QT_NS}::Gui
    ${QT_NS}::DBus
    ${QT_NS}::Quick
    ${QT_NS}::Concurrent
    ${DTK_NS}::Core
    dcc-wallpaper-utils
)

target_link_libraries(${Display_Name} PRIVATE
    ${Display_Libraries}
    $<TARGET_OBJECTS:display-wayland-client>
    PkgConfig::WaylandClient
)

dcc_install_plugin(NAME ${Display_Name} TARGET ${Display_Name})
#########################################
