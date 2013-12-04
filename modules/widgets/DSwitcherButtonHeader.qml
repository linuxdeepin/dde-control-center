import QtQuick 2.1

Rectangle {
    id: header

    property string text: dsTr("Untitled")
    property bool checked: false

    property int leftMargin: 18
    property int rightMargin: 5

    height: 30
    width: parent.width
    color: bgColor

    signal clicked

    DLabel {
        id: titleArea
        anchors.left: parent.left
        anchors.leftMargin: leftMargin
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 13
        text: header.text
    }

    Item {
        anchors.right: parent.right
        anchors.rightMargin: rightMargin
        anchors.verticalCenter: parent.verticalCenter
        height: parent.height - 2
        width: actionArea.width

        DSwitch {
            id: actionArea
            anchors.centerIn: parent
            state: header.checked ? "on" : "off"

            onStateChanged: {
                header.checked = (state == "on")
                header.clicked()
            }
        }
    }

    DSepratorHorizontal {
        anchors.bottom: parent.bottom
    }
}
