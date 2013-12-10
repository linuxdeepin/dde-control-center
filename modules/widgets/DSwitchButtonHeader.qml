import QtQuick 2.1

Rectangle {
    id: header

    property string text: dsTr("Untitled")
    property alias active: actionArea.checked

    property int leftMargin: 18
    property int rightMargin: 5
    property var dconstants: DConstants {}

    height: 30
    width: parent.width
    color: dconstants.bgColor

    signal clicked

    DssH2 {
        id: titleArea
        anchors.left: parent.left
        anchors.leftMargin: leftMargin
        anchors.verticalCenter: parent.verticalCenter
        text: header.text
    }

    Item {
        anchors.right: parent.right
        anchors.rightMargin: rightMargin
        anchors.verticalCenter: parent.verticalCenter
        height: parent.height - 2
        width: actionArea.width

        DSwitchButton {
            id: actionArea
            anchors.centerIn: parent

            onClicked: {
                header.clicked()
            }
        }
    }
}
