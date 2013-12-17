import QtQuick 2.1
import "../widgets"


Column {
    id: root

    width: 310

    property int leftPadding: 15
    property int rightPadding: 15
    property int avatarNamePadding: 30

    DBaseLine {
        leftLoader.sourceComponent: DssH1 {
            text: "Add User"
            color: "#19A9F9"
        }
    }

    DSeparatorHorizontal {}

    Rectangle {
        width: parent.width
        height: 100
        color: Qt.rgba(0, 0, 0, 0)

        DRoundImage {
            id: round_image
            roundRadius: 25
            imageSource: "/var/lib/AccountsService/icons/guest.jpg"

            property bool toggleFlag: false

            anchors.verticalCenter: parent.verticalCenter
        }

        Column {
            id: name_column

            DssH2 {
                text: "new user"
            }

            DssH3 {
                text: "User"
            }

            anchors.left: round_image.right
            anchors.leftMargin: root.avatarNamePadding
            anchors.verticalCenter: parent.verticalCenter
        }

        UserStatusButton {
            id: user_status_button
            state: "inactiveUser"

            anchors.right: parent.right
            anchors.rightMargin: root.rightPadding
            anchors.verticalCenter: parent.verticalCenter
        }
        
        anchors.left: parent.left
        anchors.leftMargin: root.leftPadding
        anchors.right: parent.right
        anchors.rightMargin: root.rightPadding
    }

    DSeparatorHorizontal {}

    Rectangle {
        width: parent.width
        height: 38
        color: "transparent"

        DLabel {
            text: "User name"
            font.pixelSize: 12

            anchors.left: parent.left
            anchors.leftMargin: 15
            anchors.verticalCenter: parent.verticalCenter
        }

        DTextInput {
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    DSeparatorHorizontal {}

    Rectangle {
        width: parent.width
        height: 38
        color: "transparent"

        DLabel {
            text: "Password"
            font.pixelSize: 12

            anchors.left: parent.left
            anchors.leftMargin: 15
            anchors.verticalCenter: parent.verticalCenter
        }

        DTextInput {
            echoMode: TextInput.Password

            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    DSeparatorHorizontal {}

    Rectangle {
        width: parent.width
        height: 38
        color: "transparent"

        DLabel {
            text: "Repeat Password"
            font.pixelSize: 12

            anchors.left: parent.left
            anchors.leftMargin: 15
            anchors.verticalCenter: parent.verticalCenter
        }

        DTextInput {
            echoMode: TextInput.Password

            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    DSeparatorHorizontal {}

    Rectangle {
        width: parent.width
        height: 38
        color: "transparent"

        DLabel {
            text: "Show Password"
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

    DSeparatorHorizontal{}

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

    DSeparatorHorizontal {}

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

    DSeparatorHorizontal{}

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
    
    DSeparatorHorizontal {}

    Rectangle {
        width: parent.width
        height: 38
        color: "transparent"

        DTextButton {
            id: cancel_button
            text: "Cancel"

            anchors.right: confirm_button.left
            anchors.rightMargin: 5
            anchors.verticalCenter: parent.verticalCenter

            onClicked: {
            }
        }

        DTextButton {
            id: confirm_button
            text: "Confirm"

            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter

            onClicked: {
            }
        }
    }
}