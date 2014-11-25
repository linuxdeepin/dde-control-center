import QtQuick 2.1
import QtQuick.Window 2.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Api.XMouseArea 1.0
import DBus.Com.Deepin.Daemon.Display 1.0
import DBus.Com.Deepin.Daemon.Dock 1.0
//import DBus.Dde.Dock.DockRegion 1.0

Window {
        id: root
        flags: Qt.Dialog | Qt.FramelessWindowHint | Qt.Popup | Qt.WindowStaysOnTopHint
        width: 180
        height: switchListView.height + titleLine.height
        x: {
            if (mouseX < displayId.primaryRect[0] + width / 2)
                return displayId.primaryRect[0]
            else if (mouseX > displayId.primaryRect[2] - width / 2)
                return displayId.primaryRect[2] - width
            else
                return mouseX - width / 2
        }

        y: displayId.primaryRect[3] - height - dockHeight - 10
        color: "transparent"

        signal itemClicked(string switchTitle, bool switchState)

        property var switchList: ListModel {}
        property int mouseX:0
        property var dockRegion: DockRegion {}
        property var dbusDockSetting: DockSetting {
            path: "/dde/dock/DockSetting"
            onDisplayModeChanged: {
                updateTimer.start()
            }
        }
        property int dockDisplayMode: dbusDockSetting.GetDisplayMode()
        property int dockHeight:70

        property int mX:0
        property int mY:0

        function updateRootY(){
            var regionValue = dockRegion.GetDockRegion()
            if (regionValue){
                if (dockHeight == regionValue[3])
                    updateTimer.start()
                else
                    dockHeight = regionValue[3]
            }
            else
                dockHeight = 70

            root.y = displayId.primaryRect[3] - switchList.count * 30 - titleLine.height - dockHeight - 10
        }

        Component.onCompleted: updateTimer.start()

        Timer {
            id:updateTimer
            repeat: false
            interval: 500
            onTriggered: {
                updateRootY()
            }
        }

        XMouseArea {
            id:xmouseArea
            onCursorMove:{
                mX = arg0
                mY = arg1
            }
        }
        Display {
            id:displayId
        }

        Rectangle {
            anchors.fill: parent
            radius: 6
            color: "#000000"
            opacity: 0.8

        }

        Column {
            anchors.fill: parent
            width: parent.width - 4
            height: switchListView.height + titleLine.height
            spacing: 0

            Rectangle {
                id:titleLine
                height: 30
                width: parent.width
                color: "transparent"

                Text {
                    id:titleText
                    text:dsTr("Notification Area Settings")
                    color: "#ffffff"
                    font.pixelSize: 11
                    anchors.centerIn: parent
                    verticalAlignment: Text.AlignVCenter
                }

                DImageButton{
                    id:closeButton
                    width: 11
                    height: 8
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: root.hide()

                    normal_image: "images/close_normal.png"
                    hover_image: "images/close_hover.png"
                    press_image: "images/close_press.png"
                }

                MouseArea {
                    property bool isPress:false
                    property int oldX:0
                    property int oldY:0
                    anchors.fill: parent
                    anchors.rightMargin: closeButton.width + 8
                    onPressed: {
                        isPress = true
                        oldX = mouseX
                        oldY = mouseY
                    }

                    onReleased: isPress = false
                    onPositionChanged: {
                        if (isPress){
                            root.x = mX - oldX
                            root.y = mY - oldY
                        }
                    }
                }

                DSeparatorHorizontal{
                    width: parent.width - 20
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    opacity: 0.7
                }
            }

            ListView {
                id:switchListView
                clip: true
                height: childrenRect.height
                width: parent.width
                model: switchList
                delegate: AppletSwitchLine{
                    height: setting_enable ? 30 : 0
                    onClicked: root.itemClicked(switchTitle,switchState)
                }
            }

        }
}
