import QtQuick 2.1
import "../widgets"

Item {
    id: root
    width: 310
    height: column.height

    property variant this_user: null

    signal avatarSet (Item item)
    signal avatarPictured (Item item, url path)

    Column {
        id: column

        AvatarView {
            id: avatar_view

            width: 310
            height: 280
            this_user: root.this_user

            onHeightChanged: {
                root.height = password_dialog.height + 38 * 2 + avatar_view.height + 2 * 3
            }

            onAvatarSet: {
                var iconFile = item.imageSource.toString().replace("file:\/\/", "")
                this_user.iconFile = iconFile
                root.avatarSet(item)
            }

            onAvatarPictured: {
                root.avatarPictured(item , path)
            }
        }

        DSeparatorHorizontal {}

        Column {
            id: edit_entries

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
                    id: auto_login_switch
                    checked: root.this_user.automaticLogin

                    onClicked: {
                        this_user.SetAutomaticLogin(checked)
                    }

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
                    id: user_type_radio

                    buttonModel: [
                        {"buttonId": "user", "buttonLabel": "User"},
                        {"buttonId": "administrator", "buttonLabel": "Administrator"},
                    ]
                    initializeIndex: root.this_user.accountType
                    onItemSelected: root.this_user.setAccountType(idx)

                    anchors.right: parent.right
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            DSeparatorHorizontal{}

            PasswordDialog {
                id: password_dialog

                onPasswordSet: {
                }

                onHeightChanged: {
                    root.height = password_dialog.height + 38 * 2 + avatar_view.height + 2 * 3
                }
            }
        }
    }
}