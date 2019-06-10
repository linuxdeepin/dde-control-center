cmake_minimum_required(VERSION 3.7)

project(dde-control-center)

find_package(DtkCMake REQUIRED)

# Install settings
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif ()

if (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

    # generate qm
    execute_process(COMMAND bash "translate_generation.sh"
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif ()

# Test architecture
execute_process(COMMAND dpkg-architecture -qDEB_BUILD_ARCH OUTPUT_VARIABLE ARCHITECTURE RESULT_VARIABLE EXIT_CODE)
if (${EXIT_CODE} EQUAL 0)
    string(STRIP ${ARCHITECTURE} ARCHITECTURE)

    if (${ARCHITECTURE} STREQUAL "sw_64")
        # add compiler flags -mieee for mathmatic
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
    endif()
endif()

set(STRONG_PASSWORD false)
set(AllowEnableMultiScaleRatio false)
set(AllowCloudSync false)

configure_file(files/dde-control-center.conf.in files/dde-control-center.conf @ONLY)

add_subdirectory("src/frame")
add_subdirectory("src/dialogs")

file(GLOB DOCS_FILES "docs/*")

## qm files
file(GLOB QM_FILES "translations/*.qm")
install(FILES ${QM_FILES} DESTINATION share/dde-control-center/translations)

#desktop
install(FILES dde-control-center.desktop DESTINATION share/applications)

#service
install(FILES com.deepin.dde.ControlCenter.service DESTINATION share/dbus-1/services)

#docs
install(FILES ${DOCS_FILES} DESTINATION share/dman/dde-control-center)

#datebases
install(FILES data/timezone.db DESTINATION share/dde-control-center)

# conf file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/files/dde-control-center.conf DESTINATION share/dde-control-center)

# polkit policy file
if (OS_DEEPIN_PROFESSIONAL)
    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        install(FILES com.deepin.controlcenter.addomain.policy DESTINATION share/polkit-1/actions)
    endif()
endif()
