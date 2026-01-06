include(CMakeParseArguments)

function(dcc_to_capitalized_string input_string output_variable)
    if(NOT input_string STREQUAL "")
        string(SUBSTRING ${input_string} 0 1 first_char)
        string(SUBSTRING ${input_string} 1 -1 rest_chars)
        string(TOUPPER ${first_char} capitalized_first_char)
        set(result "${capitalized_first_char}${rest_chars}")
    else()
        set(result "")
    endif()
    set(${output_variable} ${result} PARENT_SCOPE)
endfunction()


macro(dcc_build_plugin)
    set(oneValueArgs NAME TARGET QML_ROOT_DIR)
    set(multiValueArgs QML_FILES RESOURCE_FILES)
    set(QML_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)
    find_package(Qt6 COMPONENTS Qml)
    cmake_parse_arguments(_config "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT "${_config_NAME}" MATCHES "^[A-Za-z0-9]+$")
        message(FATAL_ERROR "Error: The name '${_config_NAME}' contains invalid characters. Only alphanumeric characters are allowed.")
    endif()

    if (DEFINED _config_QML_ROOT_DIR)
        set(QML_ROOT_DIR ${_config_QML_ROOT_DIR})
    endif()


    if(NOT _config_QML_FILES)
        file(GLOB_RECURSE _config_QML_FILES ${QML_ROOT_DIR}/*.qml ${QML_ROOT_DIR}/*.js)
    endif()

    if(NOT _config_RESOURCE_FILES)
        file(GLOB_RECURSE _config_RESOURCE_FILES ${QML_ROOT_DIR}/*.dci ${QML_ROOT_DIR}/*.svg ${QML_ROOT_DIR}/*.png ${QML_ROOT_DIR}/*.jpg ${QML_ROOT_DIR}/*.webp)
    endif()

    set(plugin_dirs ${PROJECT_BINARY_DIR}/lib/${DDE_CONTROL_CENTER_PLUGIN_DIR}/${_config_NAME}/)
    foreach(FILE_PATH ${_config_QML_FILES})
        file(RELATIVE_PATH FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR} ${FILE_PATH})
        list(APPEND QML_PATHS ${FILE_NAME})

        file(RELATIVE_PATH F_NAME ${QML_ROOT_DIR} ${FILE_PATH})

        if(${F_NAME} STREQUAL "main.qml")
            dcc_to_capitalized_string(${_config_NAME} UPPERNAME)
            message(FATAL_ERROR "Error: The filename '${F_NAME}' is deprecated. Please rename it to ${UPPERNAME}Main.qml. path:${FILE_PATH}")
        endif()
        if(NOT "${F_NAME}" MATCHES "^[A-Z]")
           dcc_to_capitalized_string(${F_NAME} UPPERNAME)
           message(FATAL_ERROR "Error: The filename '${F_NAME}' must start with an uppercase letter. Please rename it to ${UPPERNAME}. path:${FILE_PATH}")
        endif()

        set_source_files_properties(${FILE_NAME} PROPERTIES
            QT_RESOURCE_ALIAS ${F_NAME}
        )
    endforeach(FILE_PATH)

    foreach(FILE_PATH ${_config_RESOURCE_FILES})
        file(RELATIVE_PATH FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR} ${FILE_PATH})
        list(APPEND RESOURCE_PATHS ${FILE_NAME})

        file(RELATIVE_PATH F_NAME ${QML_ROOT_DIR} ${FILE_PATH})
        set_source_files_properties(${FILE_NAME} PROPERTIES
            QT_RESOURCE_ALIAS ${F_NAME}
        )
    endforeach(FILE_PATH)
    qt_policy(SET QTP0001 NEW)
    qt_policy(SET QTP0004 NEW)
    qt_add_qml_module(${_config_NAME}_qml
        PLUGIN_TARGET ${_config_NAME}_qml
        URI ${_config_NAME}
        VERSION 1.0
        QML_FILES ${QML_PATHS}
        RESOURCES ${RESOURCE_PATHS}
        OUTPUT_DIRECTORY ${plugin_dirs}
    )
    install(TARGETS ${_config_NAME}_qml DESTINATION ${DDE_CONTROL_CENTER_PLUGIN_INSTALL_DIR}/${_config_NAME})
    install(FILES ${plugin_dirs}/qmldir DESTINATION ${DDE_CONTROL_CENTER_PLUGIN_INSTALL_DIR}/${_config_NAME})

    if (DEFINED _config_TARGET)
        set_target_properties(${_config_TARGET} PROPERTIES
            PREFIX ""
            OUTPUT_NAME ${_config_NAME}
            LIBRARY_OUTPUT_DIRECTORY ${plugin_dirs}
        )
        # avoid warning when `DCC_FACTORY_CLASS` used in .cpp
        set_property(TARGET ${_config_TARGET} APPEND PROPERTY AUTOMOC_MACRO_NAMES "DCC_FACTORY_CLASS")
    endif()
endmacro()

function(dcc_install_plugin)
    dcc_build_plugin(${ARGN})
    if (DEFINED _config_TARGET)
        install(TARGETS ${_config_TARGET} DESTINATION ${DDE_CONTROL_CENTER_PLUGIN_INSTALL_DIR}/${_config_NAME})
    endif()

endfunction()

function(dcc_handle_plugin_translation)
    set(oneValueArgs NAME SOURCE_DIR)
    set(multiValueArgs QML_FILES SOURCE_FILES )
    cmake_parse_arguments(_config "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT _config_SOURCE_DIR)
        set(_config_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    if(NOT _config_QML_FILES)
        file(GLOB_RECURSE _config_QML_FILES ${_config_SOURCE_DIR}/*.qml ${_config_SOURCE_DIR}/*.js)
    endif()

    if(NOT _config_SOURCE_FILES)
        file(GLOB_RECURSE _config_SOURCE_FILES ${_config_SOURCE_DIR}/*.cpp ${_config_SOURCE_DIR}/*.h)
    endif()

    file(GLOB TRANSLATION_FILES ${_config_SOURCE_DIR}/translations/${_config_NAME}_*.ts)
    if( NOT TRANSLATION_FILES)
        set(TRANSLATION_FILES
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_en.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_en_US.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_az.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_bo.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_ca.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_es.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_fi.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_fr.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_hu.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_it.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_ja.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_ko.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_nb_NO.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_pl.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_pt_BR.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_ru.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_uk.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_zh_CN.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_zh_HK.ts
            ${_config_SOURCE_DIR}/translations/${_config_NAME}_zh_TW.ts
        )
    endif()

    # set(plugin_dirs ${PROJECT_BINARY_DIR}/plugins/${_config_NAME}/)

    # FIXME: not working on Qt 6.7
    # set_source_files_properties(${TRANSLATION_FILES}
    #     PROPERTIES OUTPUT_LOCATION "${plugin_dirs}/translations"
    # )

    add_custom_target(${_config_NAME}_translation ALL
        SOURCES ${TRANSLATION_FILES}
    )
    qt_add_translations(${_config_NAME}_translation
        TS_FILES ${TRANSLATION_FILES}
        SOURCES ${_config_QML_FILES} ${_config_SOURCE_FILES}
        LUPDATE_OPTIONS -no-obsolete -no-ui-lines -locations none
        QM_FILES_OUTPUT_VARIABLE TRANSLATED_FILES
    )
    # /usr/share/dde-control-center/translations/xxx.qm
    file(GLOB QM_FILES  "${CMAKE_CURRENT_BINARY_DIR}/*.qm")
    list(APPEND QM_FILES ${TRANSLATED_FILES})
    install(FILES ${QM_FILES} DESTINATION ${DDE_CONTROL_CENTER_TRANSLATION_INSTALL_DIR})
endfunction()
