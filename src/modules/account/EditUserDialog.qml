import QtQuick 2.1
import Deepin.Widgets 1.0

Rectangle {
    id: edit_dialog
    width: 310
    height: column.height
    color: dconstants.contentBgColor

    property var this_user: null
    property int maxHeight: 10

    signal setAvatarIconError(string uid)

    function reset() {
        password_dialog.reset()
    }

    Column {
        id: column

        AvatarView {
            id: avatar_view
            width: 310
            /* height: 326 */
            this_user: edit_dialog.this_user
            maxHeight: edit_dialog.maxHeight - edit_entries.height

            onAvatarSet: {
                var iconFile = path.toString().replace("qrc:\/\/", "")
                iconFile = iconFile.replace("file:\/\/", "")
                var right = edit_dialog.this_user.SetIconFile(iconFile)
                if (!right) {
                    doCheck(edit_dialog.this_user.iconFile)
                    edit_dialog.setAvatarIconError(edit_dialog.this_user.uid)
                }
            }

            onAvatarPictured: {
                var iconFile = path.toString().replace("qrc:\/\/", "")
                iconFile = iconFile.replace("file:\/\/", "")
                var right = edit_dialog.this_user.SetIconFile(iconFile)
                if (!right) {
                    doCheck(edit_dialog.this_user.iconFile)
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
                    text: dsTr("Auto-login")
                    font.pixelSize: 12

                    anchors.left: parent.left
                    anchors.leftMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                }

                DSwitchButton {
                    id: auto_login_switch
                    checked: edit_dialog.this_user.automaticLogin

                    onClicked: {
                        var right = edit_dialog.this_user.SetAutomaticLogin(checked)   
                        checked = Qt.binding(function () {return edit_dialog.this_user.automaticLogin})                     
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
                visible: !userIsCurrentUser(edit_dialog.this_user) && currentUserIsAdmin()

                DLabel {
                    text: dsTr("Enable")
                    font.pixelSize: 12

                    anchors.left: parent.left
                    anchors.leftMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                }

                DSwitchButton {
                    checked: !edit_dialog.this_user.locked

                    onClicked: {
                        var right = edit_dialog.this_user.SetLocked(!checked)
                        checked = Qt.binding(function () {return !edit_dialog.this_user.locked})
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
                visible: !userIsCurrentUser(edit_dialog.this_user) && currentUserIsAdmin()

                DLabel {
                    text: dsTr("Account Type")
                    font.pixelSize: 12

                    anchors.left: parent.left
                    anchors.leftMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                }

                DRadioButton {
                    id: user_type_radio

                    buttonModel: [
                        {"buttonId": "user", "buttonLabel": dsTr("Standard")},
                        {"buttonId": "administrator", "buttonLabel": dsTr("Administrator")},
                    ]
                    initializeIndex: edit_dialog.this_user.accountType
                    onItemSelected: {
                        var right = edit_dialog.this_user.SetAccountType(idx)
                        if (!right) {
                            selectItem(edit_dialog.this_user.accountType)
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

                onPasswordSet: {
                    var right = edit_dialog.this_user.SetPassword(password)
                    password_dialog.reset()
                }

                onCancelled: {
                    password_dialog.reset()
                }
            }
        }
    }
}
