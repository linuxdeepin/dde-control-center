import QtQuick 2.1
import Deepin.Widgets 1.0
import "../shared/"
import "./Widgets"

Item {

    property int contentLeftMargin: 22
    property int contentHeight: 48
    property int sliderWidth: 170
    property int leftWidth: 100
    property int centerPadding: 16


    DssTitle {
        id:remotingTitle
        anchors.top: parent.top
        text: qsTr("Feedback and help")//modulesId.moduleLocaleNames["remoting"]
    }

    DSeparatorHorizontal {
        id:separator1
        anchors.top: remotingTitle.bottom
    }

    DBaseLine {
        id: desktopBaseLine
        anchors.top: separator1.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("Remote desktop")
        }
    }

    FeatureButton {
        id: shareButton
        width: parent.width
        height: 70
        anchors.top: desktopBaseLine.bottom

        title: dsTr("Share")
        text: dsTr("Share your desktop to get remote assistance from other users")
        normalImage: "qrc:///modules/remoting/images/share_normal.png"
        hoverImage: "qrc:///modules/remoting/images/share_hover.png"
        pressImage: "qrc:///modules/remoting/images/share_press.png"
        onClicked: {
            changePage("sharePage")
        }
    }

    FeatureButton {
        id: accessButton
        width: parent.width
        height: 70
        anchors.top: shareButton.bottom

        title: dsTr("Access")
        text: dsTr("Access to the desktop shared by other users")
        normalImage: "qrc:///modules/remoting/images/access_normal.png"
        hoverImage: "qrc:///modules/remoting/images/access_hover.png"
        pressImage: "qrc:///modules/remoting/images/access_press.png"
        onClicked: {
            changePage("accessPage")
        }
    }

    DSeparatorHorizontal {
        id:separator2
        anchors.top: accessButton.bottom
    }
}
