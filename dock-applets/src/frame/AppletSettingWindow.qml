import QtQuick 2.1
import QtQuick.Window 2.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Api.XMouseArea 1.0
import DBus.Com.Deepin.Daemon.Display 1.0
import DBus.Com.Deepin.Daemon.Dock 1.0

DWindow {
    id: root
    flags: Qt.Dialog | Qt.FramelessWindowHint | Qt.Popup | Qt.WindowStaysOnTopHint
    width: titleLine.width
    height: switchListView.height + titleLine.height
    x: 0
    y: displayId.primaryRect[3] - height - dockHeight - 10
    color: "transparent"

    signal itemClicked(string switchTitle, bool switchState)

    property var switchList: ListModel {}
    property int mouseX:0
    property var dockRegion: DockRegion {}
    property int dockDisplayMode: dbusDockSetting.GetDisplayMode()
    property int dockHeight:70

    property int mX:0
    property int mY:0

    function updateRootY(){
        var regionValue = dockRegion.GetDockRegion()
        if (regionValue){
            dockHeight = regionValue[3]
        }
        else
            dockHeight = 70

        root.y = displayId.primaryRect[3] - getVisibleSwitchCount() * 30 - titleLine.height - dockHeight - 10
    }

    function getLegalX(mouseX){
        if (mouseX < displayId.primaryRect[0] + width/2)
            x = displayId.primaryRect[0] + width/2
        else if (mouseX > displayId.primaryRect[2] - width/2)
            x = displayId.primaryRect[2] - width/2
        else
            x = mouseX
        return x - width/2
    }

    function getLegalY(mouseY){
        if (mouseY > displayId.primaryRect[3] - titleLine.height - dockHeight)
            return displayId.primaryRect[3] - titleLine.height - dockHeight
        else
            return mouseY
    }

    function getVisibleSwitchCount(){
        var visibleCount = 0
        for(var i=0;i<switchList.count;i++){
            var tmpInfo = switchList.get(i)
            if(tmpInfo.setting_enable == true){
                visibleCount ++
            }
        }
        return visibleCount
    }

    function showWindow(){
        root.x = getLegalX(root.mouseX)
        updateRootY()
        root.show()
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
        radius: 2
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
            width: contentWidth < 180 ? 180 : contentWidth
            color: "transparent"

            property int contentWidth: titleText.width + closeButton.width + 50

            Text {
                id:titleText
                text: dsTr("Notification Area Settings")
                color: "#ffffff"
                font.pixelSize: 14
                anchors.centerIn: parent
                verticalAlignment: Text.AlignVCenter
            }

            DDragableArea {
                anchors.fill: parent
                window: root
            }

            DImageButton{
                id: closeButton
                anchors.right: parent.right
                anchors.rightMargin: 4
                anchors.verticalCenter: parent.verticalCenter
                onClicked: root.hide()

                normal_image: "images/close_normal.png"
                hover_image: "images/close_hover.png"
                press_image: "images/close_press.png"
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
            boundsBehavior: Flickable.StopAtBounds
            delegate: AppletSwitchLine{
                height: setting_enable ? 30 : 0
                onClicked: root.itemClicked(switchTitle,switchState)
            }
        }

    }
}
