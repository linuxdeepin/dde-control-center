import QtQuick 2.1
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Accounts 1.0
import DBus.Com.Deepin.SessionManager 1.0

Rectangle {
    id: root
    color: constants.bgColor
    width: 310
    height: 600
    property var constants: DConstants {}

    SessionManager { id: dbus_session_manager }
    Accounts { id: dbus_accounts }
    User { id: dbus_user }
    User { id: current_user; path: dbus_accounts.FindUserById(dbus_session_manager.currentUid)}

    // this function is intend to used as the entry from the home page
    function showCurrentUserDetail () {
        user_list.showCurrentUserDetail()
    }

    function userIsCurrentUser(user) {
        return user.uid == current_user.uid
    }

    function currentUserIsAdmin() {
        return current_user.accountType == 1
    }
    
    function showAddDeleteButton() {
        add_check_button.visible = true
        delete_check_button.visible = Qt.binding(function () { return dbus_accounts.userList.length != 1 })
    }
    
    function hideAddDeleteButton() {
        add_check_button.visible = false
        delete_check_button.visible = false
    }

    Column {
        id: title_column
        z: 3
        width: parent.width

        DssTitle {
            id: module_title
            text: modulesId.moduleLocaleNames["account"]
            
            Row {
                spacing: 10
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 15

                DImageCheckButton {
                    id: delete_check_button
                    visible: dbus_accounts.userList.length != 1

                    inactivatedNormalImage: "images/delete_normal.png"
                    inactivatedHoverImage: "images/delete_normal.png"
                    inactivatedPressImage: "images/delete_normal.png"

                    activatedNormalImage: "images/delete_press.png"
                    activatedHoverImage: "images/delete_press.png"
                    activatedPressImage: "images/delete_press.png"
                    
                    Behavior on opacity {
                        SmoothedAnimation { duration: 300 }
                    }

                    onClicked: {
                        if (active) {
                            user_list.allAction()
                        } else {
                            user_list.allNormal()
                        }
                    }
                }

                DImageCheckButton {
                    id: add_check_button

                    inactivatedNormalImage: "images/add_normal.png"
                    inactivatedHoverImage: "images/add_normal.png"
                    inactivatedPressImage: "images/add_normal.png"

                    activatedNormalImage: "images/add_press.png"
                    activatedHoverImage: "images/add_press.png"
                    activatedPressImage: "images/add_press.png"
                    
                    Behavior on opacity {
                        SmoothedAnimation { duration: 300 }
                    }                    

                    onClicked: {
                        if (active) {
                            main_column.state = "add_dialog"
                            root.hideAddDeleteButton()
                        } else {
                            main_column.state = "normal"
                        }
                    }

                    Connections {
                        target: main_column
                        onStateChanged: {
                            if (main_column.state == "add_dialog") {
                                add_check_button.active = true
                            } else {
                                add_check_button.active = false
                            }
                        }
                    }
                }
            }            
        }

        DSeparatorHorizontal{}
    }

    Flickable {
        anchors.top: title_column.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        contentWidth: main_column.childrenRect.width
        contentHeight: main_column.childrenRect.height + guest_user.height

        Column {
            id: main_column
            state: "normal"

            AddUserDialog {
                id: add_user_dialog

                onCancelled: {
                    main_column.state = "normal"
                    root.showAddDeleteButton()
                    add_user_dialog.reset()
                }

                onConfirmed: {
                    root.showAddDeleteButton()                    
                    var result = dbus_accounts.CreateUser(userInfo.userName, userInfo.userName, userInfo.userAccountType)
                    var new_user = result[0]
                    var right = result[1]
                    if (!right) {
                        add_user_dialog.warnUserName()
                    } else {
                        dbus_user.path = new_user
                        /* dbus_user.passwordMode = 2 // i think this nonsense too, but the fact is this help a lot >_< */
                        /* // The user should be in a group named "nopasswdlogin" before we set his password, */
                        /* // but a fresh _new_ user is not in that group(weird), so we should set it first. */
                        dbus_user.SetPassword(userInfo.userPassword, "")
                        dbus_user.SetIconFile(userInfo.userIconFile)
                        dbus_user.SetAccountType(userInfo.userAccountType)
                        dbus_user.SetAutomaticLogin(userInfo.userAutoLogin)

                        main_column.state = "normal"
                        add_user_dialog.reset()
                    }
                }
            }

            UserList {
                id: user_list

                Connections {
                    target: dbus_accounts
                    onUserAdded: {
                        user_list.addUser(arg0)
                    }
                    onUserDeleted: {
                        user_list.deleteUser(arg0)
                    }
                }

                onHideAllPrivate: {
                    guest_user.visible = false
                    hideAddDeleteButton()
                }

                onShowAllPrivate: {
                    guest_user.visible = true
                    showAddDeleteButton()
                }
                
                onAllNormal: {
                    delete_check_button.active = false
                    root.showAddDeleteButton()
                }
                
                onActionButtonClicked: root.hideAddDeleteButton()
            }

            states: [
                State {
                    name: "normal"

                    PropertyChanges {
                        target: user_list
                        visible: true
                    }
                    PropertyChanges {
                        target: guest_user
                        visible: true
                    }
                    PropertyChanges {
                        target: add_user_dialog
                        visible: false
                    }
                },
                State {
                    name: "add_dialog"

                    PropertyChanges {
                        target: user_list
                        visible: false
                    }
                    PropertyChanges {
                        target: guest_user
                        visible: false
                    }
                    PropertyChanges {
                        target: add_user_dialog
                        visible: true
                    }
                }
            ]
        }

        GuestUser {
            id: guest_user
            width: parent.width

            onExpandedChanged: {
                user_list.visible = !expanded
            }

            Behavior on anchors.top {
                NumberAnimation { duration: 100 }
            }

            anchors.top: main_column.bottom

            leftPadding: user_list.leftPadding
            rightPadding: user_list.rightPadding
            nameLeftPadding: user_list.nameLeftPadding
        }
    }
}
