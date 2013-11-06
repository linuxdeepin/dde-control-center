import QtQuick 2.0

Item {
    id: container

    property string text: "Button"
    property string moduleID: "Button"
    signal clicked
    property alias containsMouse: mouseArea.containsMouse
    property alias pressed: mouseArea.pressed
    implicitHeight: 48
    height: implicitHeight
    width: parent.width

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: container.clicked()
        hoverEnabled: true
    }

    Image { 
        id: moduleID
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: container.verticalCenter
        source: "./images/preferences-"+module_id+".png"
        scale: 1/1.5
    }

    Text {
        id: buttonLabel
        anchors.left: parent.left
        anchors.leftMargin: 88
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: container.verticalCenter
        text: container.text
        color: "#eaeaea"
        font.pixelSize: 22
        styleColor: "white"
        //style: Text.Raised
    }
}
