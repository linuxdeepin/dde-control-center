import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Locale 1.0
import Deepin.Widgets 1.0

Window {
    id: trayIconTip
    width: trayIconTipText.width + 52
    height: 44
    flags: Qt.Popup
    visible: false

    property alias text: trayIconTipText.text
    property alias timeOutHide: hide
    property bool isHover: false

    color: Qt.rgba(0, 0, 0, 0)

    Timer {
        id: hide
        interval: 300
        running: false
        repeat: false
        onTriggered: {
            if (!trayIconTip.isHover){
                trayIconTip.visible = false
            }
        }
    }

    Behavior on y {
        SmoothedAnimation { duration: 300 }
    }

    RectWithCorner {
        id: trayIconTipArrowRect
        x: 0
        y: 0
        cornerDirection: "right"
        rectWidth: parent.width
        rectHeight: parent.height
        cornerPos: rectHeight/2
        cornerWidth: 16
        cornerHeight: 8
    }

    Text {
        id: trayIconTipText
        anchors.verticalCenter: trayIconTipArrowRect.verticalCenter
        anchors.horizontalCenter: trayIconTipArrowRect.horizontalCenter
        anchors.horizontalCenterOffset: - 4
        font.pixelSize: 13
        color: "white"
    }
}
