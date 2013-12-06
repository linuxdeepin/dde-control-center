import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../widgets"

RowLayout {
    id: header

    anchors.left:parent.left
    anchors.right:parent.right
    anchors.leftMargin: 10

    height: 50
    Label {
        text: dsTr("Network Settings")
        color: fgColor
        font.pointSize: 12
        MouseArea {
            anchors.fill: parent
            onClicked: {
                stackView.pop(null)
            }
        }
    }
    DTextButton {
        text: dsTr("Create Connections")
        anchors.right: abc.left
    }
    DTextButton {
        anchors.right: parent.right
        id: abc
        text: "i"
        anchors.rightMargin: 5
        MouseArea {
            anchors.fill: parent
            onClicked: {
                stackView.push(Qt.resolvedUrl("Info.qml"))
            }
        }
    }
}
