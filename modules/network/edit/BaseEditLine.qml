import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseLine {
    id: editLine
    objectName: "BaseEditLine"
    
    property var connectionSession
    property var availableSections
    property var availableKeys
    property var connectionData
    property var errors
    property string section
    property string key
    property string text
    property bool alwaysUpdate: false
    
    // TODO
    color: dconstants.contentBgColor
    visible: rightLoader.item.visible
    // visible: false
    // Binding on visible {
    //     value: isChildVisible()
    // }
    // function isChildVisible() {
    //     if (rightLoader.item && rightLoader.item.visible) {
    //         return true
    //     } else {
    //         return false
    //     }
    // }
    leftMargin: contentLeftMargin
    leftLoader.sourceComponent: DssH2{
        text: editLine.text
    }
    onTextChanged: {
        print("<<<<", editLine.text)
    }
}
