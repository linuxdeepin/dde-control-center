import QtQuick 2.1
import "../widgets"

Item {
    id: root
    width: 310
    height: column.height

    property variant this_user: null

    /* item == null indicates that avatar set by other programs */
    signal avatarSet (Item item)

    DColumn {
        id: column

        AvatarView {
            id: avatar_view

            width: 310
            height: 300

            property variant lastClickedAvatar: null

            onAvatarSet: {
                lastClickedAvatar = item
                this_user.SetIconFile(item.imageSource.toString().replace("file:\/\/", ""))
            }

            Connections {
                target: root.this_user

                onChanged: {
                    root.avatarSet(avatar_view.lastClickedAvatar)
                    avatar_view.lastClickedAvatar = null

                    //TODO IconView selection
                }
            }
        }

        DScrollWidget {
            width: parent.width + 15
            height: (38 + 2) * 4

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
                        checked: root.this_user

                        onClicked: {
                            this_user.SetAutomaticLogin(checked)
                        }

                        Connections {
                            target: root.this_user

                            onChanged: {
                                auto_login_switch.checked = root.this_user.automaticLogin
                            }
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
                        text: "Face Recognition"
                        font.pixelSize: 12

                        anchors.left: parent.left
                        anchors.leftMargin: 15
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    DSwitchButton {

                        Connections {
                            target: root.this_user

                            onChanged: {

                            }
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

                        Connections {
                            target: root.this_user

                            onChanged: {
                                user_type_radio.selectItem(root.this_user.accountType)
                            }
                        }
                        
                        anchors.right: parent.right
                        anchors.rightMargin: 15
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                DSeparatorHorizontal{}

                PasswordDialog {
                    id: password_dialog
                }
            }
        }

        Component.onCompleted: {
            height = password_dialog.height + 38 * 3 + avatar_view.height + 2 * 4
            parent.height = height
        }
    }
}