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
            height: 300

            onAvatarSet: {
                var iconFile = item.imageSource.toString().replace("file:\/\/", "")
                this_user.iconFile = iconFile
                root.avatarSet(item)                
            }

            onAvatarPictured: {
                root.avatarPictured(item , path)                
                /* var iconFile = path.toString().replace("file:\/\/", "") */
                /* this_user.SetIconFile(iconFile) */
                /* print(this_user.iconFile) */
                /* print(iconFile) */
                /* if (this_user.iconFile == iconFile) { */
                /*     root.avatarPictured(item , path) */
                /* } */
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

            /* DSeparatorHorizontal{} */

            /* Rectangle { */
            /*     width: parent.width */
            /*     height: 38 */
            /*     color: "transparent" */

            /*     DLabel { */
            /*         text: "Face Recognition" */
            /*         font.pixelSize: 12 */

            /*         anchors.left: parent.left */
            /*         anchors.leftMargin: 15 */
            /*         anchors.verticalCenter: parent.verticalCenter */
            /*     } */

            /*     DSwitchButton { */
            /*         anchors.right: parent.right */
            /*         anchors.rightMargin: 15 */
            /*         anchors.verticalCenter: parent.verticalCenter */
            /*     } */
            /* } */

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
                    print(password)
                }

                onHeightChanged: {
                    root.height = password_dialog.height + 38 * 2 + avatar_view.height + 2 * 3
                }
            }
        }

        Component.onCompleted: {
            height = password_dialog.height + 38 * 2 + avatar_view.height + 2 * 3
            parent.height = height
        }
    }
}