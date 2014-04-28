import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseLine {
    id: root
    height: 38
    property var section: "general"
    property var key: "id"
    property var errors: connectionSessionObject.errors[section]
    function isValueError() {
        return errors[root.key] ? true : false
    }

    leftLoader.sourceComponent: Row{
        spacing: 6
        DssH2{
            text: dsTr("Connection Name:")
        }
        DTextInput{
            textInput.color: dconstants.fgColor
            text: generalGetKey(section, key)
            onTextChanged: {
                generalSetKey(section, key, text)
            }

            // TODO error state
            property color normalColor: dconstants.contentBgColor
            property color errorColor: "#F48914"
            property bool showErrorConditon
            property bool showError: showErrorConditon && isValueError()
            onActiveFocusChanged: {
                if (!activeFocus) {
                    showErrorConditon = true
                }
            }
            onShowErrorChanged: {
                root.border.color = showError ? errorColor : normalColor
            }
        }
    }
}

