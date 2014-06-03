import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    width: childrenRect.width
    height: 30
    property alias text: content.text

    DssH2 { 
        id: content
        anchors.verticalCenter: parent.verticalCenter
    }
}
