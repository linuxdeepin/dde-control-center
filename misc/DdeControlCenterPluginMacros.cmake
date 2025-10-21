include(CMakeParseArguments)

macro(dcc_build_plugin)
    set(oneValueArgs NAME TARGET QML_ROOT_DIR)
    set(qml_root_dir ${CMAKE_CURRENT_SOURCE_DIR}/qml)
    cmake_parse_arguments(_config "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    if (DEFINED _config_QML_ROOT_DIR)
        set(qml_root_dir ${_config_QML_ROOT_DIR})
    endif()
    file(GLOB_RECURSE qml_files ${qml_root_dir}/*)
    add_custom_target(${_config_NAME}_qml ALL
        SOURCES ${qml_files}
    )
    set(plugin_dirs ${PROJECT_BINARY_DIR}/lib/${DDE_CONTROL_CENTER_PLUGIN_DIR}/${_config_NAME}/)
    add_custom_command(TARGET ${_config_NAME}_qml POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${qml_root_dir} ${plugin_dirs}
    )

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
    install(DIRECTORY ${plugin_dirs} DESTINATION ${DDE_CONTROL_CENTER_PLUGIN_INSTALL_DIR}/${_config_NAME})
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
