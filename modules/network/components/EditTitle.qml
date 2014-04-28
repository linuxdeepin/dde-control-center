import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseLine {
    id: root
    height: 38
    property var section: "general"
    property var key: "id"
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
        }
    }
}

