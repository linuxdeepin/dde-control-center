cmake_minimum_required(VERSION 3.18)
set(pluginName datetime)
file(GLOB_RECURSE datetime_SRCS
    "operation/*.cpp"
    "operation/*.h"
    "operation/timezoneMap/*.h"
    "operation/timezoneMap/*.cpp"
    "operation/qrc/datetime.qrc"
)

# List of QML files with their source paths
set(QML_SOURCE_FILES
    qml/ComboLabel.qml
    qml/datetimeMain.qml
    qml/datetime.qml
    qml/DateTimeSettingDialog.qml
    qml/LangAndFormat.qml
    qml/LangsChooserDialog.qml
    qml/RegionFormatDialog.qml
    qml/RegionsChooserWindow.qml
    qml/SearchableListViewPopup.qml
    qml/SpinboxEx.qml
    qml/TimeAndDate.qml
    qml/TimezoneClock.qml
    qml/TimezoneDialog.qml
)

# Set QT_RESOURCE_ALIAS for each QML file to remove the 'qml/' prefix
foreach(_qml_file IN LISTS QML_SOURCE_FILES)
    get_filename_component(_qml_filename "${_qml_file}" NAME)
    set_source_files_properties("${_qml_file}" PROPERTIES
        QT_RESOURCE_ALIAS "${_qml_filename}"
    )
endforeach()

qt_add_qml_module(${pluginName}
    URI "org.deepin.dcc.datetime"
    QML_FILES ${QML_SOURCE_FILES}
    SOURCES ${datetime_SRCS}
    NO_PLUGIN
)

find_package(ICU COMPONENTS i18n uc)

set(dateTime_Libraries
    ${DCC_FRAME_Library}
    ${QT_NS}::DBus
    ${QT_NS}::Concurrent
     ${DTK_NS}::Gui
     ICU::i18n
     ICU::uc
)

target_include_directories(${pluginName} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/operation
)

target_link_libraries(${pluginName} PRIVATE
    ${dateTime_Libraries}
    dcc-shared-utils
)

dcc_install_plugin(NAME ${pluginName} TARGET ${pluginName} DISABLE_QML_COPY)

# keyboard_language_*.ts生成使用脚本misc/translate_language2ts.sh手动执行
