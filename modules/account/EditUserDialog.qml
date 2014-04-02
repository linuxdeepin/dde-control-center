import QtQuick 2.1
import Deepin.Widgets 1.0

Rectangle {
    id: root
    width: 310
    height: column.height
    color: dconstants.contentBgColor

    property var this_user: null

    Column {
        id: column

        AvatarView {
            id: avatar_view
            width: 310
            height: 326
            this_user: root.this_user

            onAvatarSet: {
                if (checkPolkitAuth()) {
                    var iconFile = path.toString().replace("file:\/\/", "")
                    this_user.SetIconFile(iconFile)
                }
            }

            onAvatarPictured: {
                if (checkPolkitAuth()) {
                    var iconFile = path.toString().replace("file:\/\/", "")
                    this_user.SetIconFile(iconFile)
                }
            }
        }

        DSeparatorHorizontal {}

        Column {
            id: edit_entries

            Item {
                width: parent.width
                height: 38

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
                        if (checkPolkitAuth()) {
                            this_user.SetAutomaticLogin(checked)                           
                        }
                    }

                    anchors.right: parent.right
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            DSeparatorHorizontal{
                visible: account_enabled_switch.visible
            }
            
            Item {
                id: account_enabled_switch
                width: parent.width
                height: 38
                visible: !userIsCurrentUser(this_user) && currentUserIsAdmin()

                DLabel {
                    text: dsTr("Enable User")
                    font.pixelSize: 12

                    anchors.left: parent.left
                    anchors.leftMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                }

                DSwitchButton {
                    checked: !root.this_user.locked

                    onClicked: {
                        if (checkPolkitAuth()) {
                            this_user.SetLocked(!checked)
                        }
                    }

                    anchors.right: parent.right
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                }                
            }            
            
            DSeparatorHorizontal{
                visible: account_type_radio.visible
            }            

            Item {
                id: account_type_radio
                width: parent.width
                height: 38
                visible: !userIsCurrentUser(this_user) && currentUserIsAdmin()

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
                    onItemSelected: if(checkPolkitAuth()) {root.this_user.SetAccountType(idx)}

                    anchors.right: parent.right
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            DSeparatorHorizontal{}

            PasswordDialog {
                id: password_dialog

                onPasswordSet: {
                    if (checkPolkitAuth()) {
                        password_dialog.reset()
                        /* dbus_user.passwordMode = 2 // i think this nonsense too, but the fact is this help a lot >_< */
                        /* // The user should be in a group named "nopasswdlogin" before we set his password, */
                        /* // but a fresh _new_ user is not in that group(weird), so we should set it first. */
                        dbus_user.SetPassword(password, "")
                    }
                }

                onCancelled: {
                    password_dialog.reset()
                }
            }
        }
    }
}