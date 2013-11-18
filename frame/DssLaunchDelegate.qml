import QtQuick 2.0

Rectangle {
    id: container
    property Item exampleItem
    width: ListView.view.width
    height: button.implicitHeight + 10
    color: "#252627"

    Image {
        id: image
        opacity: 0.7
        Behavior on opacity {NumberAnimation {duration: 100}}
        source: "images/next.png"
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 16
    }

    DssLaunchListItem {
        id: button
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right:image.left
        text: name
        moduleID: module_id
        onClicked: {
            exampleItem.exampleUrl = url; 
            title.text = name
        }
    }

    Rectangle {
        height: 1
        color: "#0b0809"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 1
        anchors.left: parent.left
        anchors.right: parent.right
    }
    Rectangle {
        height: 1
        color: "#3b3c3d"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
    }
}
