import QtQuick 2.1
import "../widgets"

Item {
    id: root
    width: 310
    height: column.height

    DColumn {
        id: column

        AvatarView {
            id: avatar_view

            width: 310
            height: 300
        }

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DLabel {
                text: "Auto-login"
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DSwitchButton {
                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DLabel {
                text: "Face Recognition"
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DSwitchButton {
                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DLabel {
                text: "User Group"
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DRadioButton {

                buttonModel: [
                    {"buttonId": "administrator", "buttonLabel": "Administrator"},
                    {"buttonId": "user", "buttonLabel": "User"},
                ]

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        PasswordDialog {
            id: password_dialog
        }

        Component.onCompleted: {
            height = password_dialog.height + 38 * 3 + avatar_view.height + 2 * 4
            parent.height = height
        }
    }
}