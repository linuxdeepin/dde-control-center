
import QtQuick 2.2

import Deepin.Widgets 1.0

Item {
    Rectangle {
        width: parent.width
        height: childrenRect.height
        anchors.top: parent.top
        color: DConstants.contentBgColor

        Text {
            width: parent.width - 40
            height: 70
            lineHeight: 1.3
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.Wrap
            font.pixelSize: 12
            color: DConstants.tuhaoColor
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            text: dsTr("No network connections are available, please retry...")
        }
    }
}
