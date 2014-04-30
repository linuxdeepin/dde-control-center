import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Locale 1.0
import Deepin.Widgets 1.0

Window {
    id: trayIconTip
    width: trayIconTipArrowRect.width
    height: trayIconTipArrowRect.height
    flags: Qt.Popup
    visible: false

    property alias text: trayIconTipText.text
    property alias timeOutHide: hide
    property bool isHover: false

    color: Qt.rgba(0, 0, 0, 0)

    Timer {
        id: hide
        interval: 200
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
        enabled: trayIconTip.visible
    }

    Row {
        id: trayIconTipArrowRect

        Image{
            source: "images/tooltip_left.png"
        }

        Image{
            width: trayIconTipText.width + 20
            source: "images/tooltip_middle.png"
        }
        
        Image{
            source: "images/tooltip_right.png"
        }

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
