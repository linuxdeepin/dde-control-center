cmake_minimum_required(VERSION 3.18)
set(DCCFrame_Name ${DCC_Library})
file(GLOB DCCFrame_SRCS
    "include/*.h"
    "*.cpp"
)

add_library(${DCCFrame_Name} SHARED
    ${DCCFrame_SRCS}
)

target_include_directories(${DCCFrame_Name} PRIVATE
    include
)

set(DCCFrame_Libraries
    ${DTK_NS}::Gui
    ${QT_NS}::Gui
    ${QT_NS}::Quick
)

target_link_libraries(${DCCFrame_Name} PRIVATE
    ${DCCFrame_Libraries}
)

set_target_properties(${DCCFrame_Name} PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION}
    SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR}
    OUTPUT_NAME dde-control-center
    EXPORT_NAME Control-Center
)

install(TARGETS ${DCCFrame_Name}
        EXPORT DdeControlCenterInterfaceTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dde-control-center
)

install(EXPORT DdeControlCenterInterfaceTargets
        FILE DdeControlCenterInterfaceTargets.cmake
        NAMESPACE Dde::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter
)
