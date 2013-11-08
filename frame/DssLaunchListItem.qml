import QtQuick 2.0

Item {
    id: container

    property string text: "Button"
    property string moduleID: "Button"
    signal clicked
    property alias containsMouse: mouseArea.containsMouse
    property alias pressed: mouseArea.pressed
    implicitHeight: 36
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
        anchors.leftMargin: 10
        anchors.verticalCenter: container.verticalCenter
        source: "./images/preferences-"+module_id+".png"
        scale: 0.5
    }

    Text {
        id: buttonLabel
        anchors.left: parent.left
        anchors.leftMargin: 60
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: container.verticalCenter
        text: container.text
        color: "#eaeaea"
        font.pixelSize: 18
        styleColor: "white"
        //style: Text.Raised
    }
}
