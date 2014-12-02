import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    height: 60

    property string missingTypeName: ""
    
    leftLoader.sourceComponent: Column {
        width: root.width - root.leftMargin - root.rightMargin
        spacing: 2

        DssH3 {
            text: dsTr("Current system lacks component for %1").arg(root.missingTypeName)
            width: parent.width
            wrapMode: Text.WordWrap
            font.pixelSize: 12
        }

        DTextAction {
            text: dsTr("Click to install")
            hasUnderline: true
            label.color: errorColor
            width: label.width
            onClicked: {
                mainObject.installPackage(root.cacheValue)
            }
        }
    }
}    

