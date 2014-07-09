import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    id: root
    width: childrenRect.width
    height: 30
    visible: value && value != "" && visibleCondition 
    property bool visibleCondition: true
    property alias label: left.text
    property alias value: right.text

    Row {
        height: parent.height
        DssH2 { 
            id: left
            width: 100
            anchors.verticalCenter: parent.verticalCenter
        }
        DssH2 { 
            id: right
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
