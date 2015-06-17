import QtQuick 2.1
import Deepin.Widgets 1.0

Column {
    id: root

    signal cancel
    signal confirm (bool deleteFiles)

    Item {
        width: parent.width
        height: parent.height / 2

        DRadioButton {
            id: radio_button

            buttonModel: [
                {"buttonId": "keep_files", "buttonLabel": dsTr("Keep user's files")},
                {"buttonId": "delete_files", "buttonLabel": dsTr("Delete user's files")},
            ]

            anchors.centerIn: parent
        }
    }

    Item {
        width: parent.width
        height: parent.height / 2

        DTextButton {
            id: cancel_button
            text: dsTr("Cancel")

            anchors.right: confirm_button.left
            anchors.rightMargin: 5
            anchors.verticalCenter: parent.verticalCenter

            onClicked: {
                root.cancel()
            }
        }

        DTextButton {
            id: confirm_button
            text: dsTr("Confirm")

            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter

            onClicked: {
                root.confirm(radio_button.currentIndex == 1)
            }
        }
    }
}
