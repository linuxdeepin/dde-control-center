import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
    id: themeItem

    property string itemValue: item_value

    states: [
        State {
            name: "hovered"
            PropertyChanges { target: itemNameArea; color: dconstants.hoverColor }
            PropertyChanges { target: itemThumbnailBox; border.color: dconstants.hoverColor }
        }
    ]

    Rectangle{
        id: hoverBackground
        anchors.fill: parent
        color: "#101010"
        radius: 3
        visible: false
    }
    
    Rectangle {
        id: itemThumbnailBox
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 8
        width: 130
        height: 74
        color: "transparent"
        border.width: 1
        border.color: dconstants.fgColor

        Image {
            id: itemThumbnailImage
            anchors.centerIn: parent
            source: dataDir + item_img_url
            width: parent.width - 2
            height: parent.height - 2
        }
    }

    DssH2 {
        id: itemNameArea
        anchors.top: itemThumbnailBox.bottom
        anchors.topMargin: 6
        anchors.horizontalCenter: parent.horizontalCenter
        text: item_name
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onClicked: {
        }

        onEntered: {
            parent.state = "hovered"
        }

        onExited: {
            parent.state = ""
        }

        onPressed: {
        }

        onReleased: {
        }
    }

}
