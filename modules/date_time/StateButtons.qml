import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

Item {
    id: stateButtons
    width: actionButtons.width
    height: parent.height

    property string completeLabel: dsTr("Done")
    property string addLabel: dsTr("Add")
    property string currentActionStateName: ""
    property alias deleteButton: deleteButton
    property alias addButton: addButton

    DTextButton {
        id: completeButton
        text: currentActionStateName == "addButton" ? addLabel : completeLabel
        opacity: currentActionStateName != "" ? 1 : 0
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        visible: opacity == 0 ? false : true

        Behavior on opacity {
            PropertyAnimation { duration:200 }
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
