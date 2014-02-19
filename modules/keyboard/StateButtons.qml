import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

Item {
    id: stateButtons
    width: actionButtons.width
    height: parent.height

    property string completeButtonLabel: dsTr("Finish")
    property string currentActionStateName: ""
    property alias deleteButton: deleteButton
    property alias addButton: addButton

    DTextButton {
        id: completeButton
        text: completeButtonLabel
        opacity: stateButtons.currentActionStateName == "" ? 0 : 1
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        visible: opacity == 0 ? false : true

        Behavior on opacity {
            PropertyAnimation { duration:100 }
        }

        onClicked: {
            stateButtons.currentActionStateName = ""
        }
    }

    Row {
        id: actionButtons
        anchors.verticalCenter: parent.verticalCenter
        width: childrenRect.width
        spacing: 4
        opacity: stateButtons.currentActionStateName == "" ? 1 : 0
        visible: opacity == 0 ? false : true

        Behavior on opacity {
            PropertyAnimation { duration:200 }
        }

        DssDeleteButton {
            id: deleteButton
            onClicked: {
                stateButtons.currentActionStateName = "deleteButton"
            }
        }

        DssAddButton {
            id: addButton
            onClicked: {
                stateButtons.currentActionStateName = "addButton"
            }
        }
    }

}
