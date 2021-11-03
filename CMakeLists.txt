cmake_minimum_required(VERSION 3.7)

project(dde-control-center)

find_package(DtkCMake REQUIRED)
include(Dart)
include(CTest)

add_compile_definitions(CVERSION="${CVERSION}")

# Install settings
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif ()

# 增加安全编译参数
set(SECURITY_COMPILE, "-fstack-protector-strong -D_FORTITY_SOURCE=1 -z noexecstack -pie -fPIC -z lazy")
set(CMAKE_CXX_FLAGS, "${CMAKE_CXX_FLAGS} ${SECURITY_COMPILE}")

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
set(AllowCloudSync true)

configure_file(files/dde-control-center.conf.in files/dde-control-center.conf @ONLY)

add_definitions(-DWINDOW_MODE)

add_subdirectory("src/frame")
add_subdirectory("src/reboot-reminder-dialog")
add_subdirectory("src/develop-tool")
add_subdirectory("tests")

if (NOT DEFINED DISABLE_RECOVERY)
    add_subdirectory("abrecovery")
endif()

file(GLOB DOCS_FILES "docs/*")
file(GLOB HEADERS "include/*")

## qm files
file(GLOB QM_FILES "translations/*.qm")
install(FILES ${QM_FILES} DESTINATION share/dde-control-center/translations)

#desktop
install(FILES dde-control-center.desktop DESTINATION share/applications)
install(FILES dde-control-center-wapper DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

#schemas
install(FILES com.deepin.dde.control-center.gschema.xml DESTINATION share/glib-2.0/schemas)
install(CODE "execute_process(COMMAND glib-compile-schemas ${CMAKE_INSTALL_PREFIX}/share/glib-2.0/schemas)")

#dde-grand-search-daemon plugins
install(FILES com.deepin.dde-grand-search.dde-control-center-setting.conf DESTINATION lib/${CMAKE_LIBRARY_ARCHITECTURE}/dde-grand-search-daemon/plugins/searcher)

#service
install(FILES com.deepin.dde.ControlCenter.service DESTINATION share/dbus-1/services)

#docs
install(FILES ${DOCS_FILES} DESTINATION share/dman/dde-control-center)

#datebases
install(FILES data/timezone.db DESTINATION share/dde-control-center)

# dict file
install(FILES data/MainEnglishDictionary_ProbWL.txt DESTINATION share/dict)

# conf file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/files/dde-control-center.conf DESTINATION share/dde-control-center)

# polkit policy file
install(FILES com.deepin.controlcenter.develop.policy DESTINATION share/polkit-1/actions)
if (OS_DEEPIN_PROFESSIONAL)
    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        install(FILES com.deepin.controlcenter.addomain.policy DESTINATION share/polkit-1/actions)
    endif()
endif()

# dev files
set(INCLUDE_INSTALL_DIR include/dde-control-center)
install(DIRECTORY ${HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR})

include(CMakePackageConfigHelpers)
configure_package_config_file(DdeControlCenterConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter
    PATH_VARS INCLUDE_INSTALL_DIR)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DdeControlCenterConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DdeControlCenter)
