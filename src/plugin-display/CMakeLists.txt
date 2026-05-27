cmake_minimum_required(VERSION 3.18)

find_package(${QT_NS} REQUIRED COMPONENTS WaylandClient)
find_package(PkgConfig REQUIRED)
find_package(TreelandProtocols REQUIRED)
pkg_check_modules(WaylandClient REQUIRED IMPORTED_TARGET wayland-client)
pkg_check_modules(WLR_PROTOCOLS REQUIRED wlr-protocols)

execute_process(
    COMMAND pkg-config --variable=pkgdatadir wlr-protocols
    OUTPUT_VARIABLE WLR_PROTOCOLS_XML_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

file(GLOB_RECURSE LIBWAYQT_SRCS
    "wayland/libwayqt/*.cpp"
    "wayland/libwayqt/*.h"
)

add_library(libwayqt6
    OBJECT
    ${LIBWAYQT_SRCS}
)

qt_generate_wayland_protocol_client_sources(libwayqt6
    FILES
        ${WLR_PROTOCOLS_XML_DIR}/unstable/wlr-output-management-unstable-v1.xml
        ${TREELAND_PROTOCOLS_DATA_DIR}/treeland-output-manager-v1.xml
        ${TREELAND_PROTOCOLS_DATA_DIR}/treeland-virtual-output-manager-v1.xml
        ${TREELAND_PROTOCOLS_DATA_DIR}/treeland-wallpaper-manager-unstable-v1.xml
)

# fix arm64 build failed relocation R_AARCH64_ADR_PREL_PG_HI21 against symbol
target_compile_options(libwayqt6 PRIVATE "-fpic")

target_include_directories(libwayqt6
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/wayland/libwayqt"
    PRIVATE
        ${Qt6Gui_PRIVATE_INCLUDE_DIRS}
)

target_link_libraries(libwayqt6
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
    $<TARGET_PROPERTY:libwayqt6,INTERFACE_INCLUDE_DIRECTORIES>
)

set(Display_Libraries
    ${DCC_FRAME_Library}
    ${QT_NS}::Gui
    ${QT_NS}::DBus
    ${QT_NS}::Quick
    ${DTK_NS}::Core
)

target_link_libraries(${Display_Name} PRIVATE
    ${Display_Libraries}
    $<TARGET_OBJECTS:libwayqt6>
    PkgConfig::WaylandClient
)

dcc_install_plugin(NAME ${Display_Name} TARGET ${Display_Name})
#########################################
