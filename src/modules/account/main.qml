import QtQuick 2.1
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Accounts 1.0
import DBus.Com.Deepin.SessionManager 1.0
import "../shared"

Rectangle {
    id: root
    color: constants.bgColor
    width: 310
    height: 600
    property var constants: DConstants {}

    property bool inUserCreating: false

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

    property int lastContentY: 0
    function scrollToTop() {
        lastContentY = flickable.contentY
        flickable.contentY = 0
    }

    function scrollToLastPosition() {
        flickable.contentY = lastContentY
    }

    Column {
        id: title_column
        z: 3
        width: parent.width

        DssTitle {
            id: module_title
            text: modulesId.moduleLocaleNames["account"]

            ItemTooltip {
                id:userTooltip
                anchors.right: buttonRow.left
                anchors.rightMargin: 5
                anchors.verticalCenter: parent.verticalCenter
                width: module_title.width / 2
                delayShowInterval: 500
                textItem.font.pixelSize: 12
            }

            Row {
                id: buttonRow
                spacing: 10
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 15
                visible: !inUserCreating


                DImageCheckButton {
                    id: delete_check_button
                    visible: dbus_accounts.userList.length != 1

                    inactivatedNormalImage: "images/delete_normal.png"
                    inactivatedHoverImage: "images/delete_hover.png"
                    inactivatedPressImage: "images/delete_press.png"

                    activatedNormalImage: "images/delete_active.png"
                    activatedHoverImage: "images/delete_active.png"
                    activatedPressImage: "images/delete_active.png"

                    Behavior on opacity {
                        SmoothedAnimation { duration: 300 }
                    }

                    onClicked: {
                        if (active) {
                            user_list.allAction()
                        } else {
                            user_list.allNormal()
                        }

                        userTooltip.hideTipImmediately()
                    }
                    onHoverChanged: {
                        if (hover){
                            userTooltip.tooltip = dsTr("Delete Account")
                            userTooltip.showTip()
                        }
                        else{
                            userTooltip.hideTip()
                        }
                    }
                }

                DImageCheckButton {
                    id: add_check_button

                    inactivatedNormalImage: "images/add_normal.png"
                    inactivatedHoverImage: "images/add_hover.png"
                    inactivatedPressImage: "images/add_press.png"

                    activatedNormalImage: "images/add_active.png"
                    activatedHoverImage: "images/add_active.png"
                    activatedPressImage: "images/add_active.png"

                    Behavior on opacity {
                        SmoothedAnimation { duration: 300 }
                    }

                    onClicked: {
                        user_list.allNormal()

                        if (active) {
                            main_column.state = "add_dialog"
                            root.scrollToTop()
                            root.hideAddDeleteButton()
                        } else {
                            main_column.state = "normal"
                            root.scrollToLastPosition()
                        }

                        userTooltip.hideTipImmediately()
                    }
                    onHoverChanged: {
                        if (hover){
                            userTooltip.tooltip = dsTr("Add Account")
                            userTooltip.showTip()
                        }
                        else{
                            userTooltip.hideTip()
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

            DTextButton {
                id:user_list_button
                text:dsTr("User List")
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 15
                visible: inUserCreating || add_check_button.visible || (add_user_dialog.visible && !add_check_button.visible)?false:true
                onClicked: {
                    user_list.backToNormal()

                    guest_user.setExpanded(false)

                    guest_user.visible = true
                    guest_user.height = guest_user.childrenRect.height
                    showAddDeleteButton()
                }
            }
        }

        DSeparatorHorizontal{}
    }

    Flickable {
        id: flickable
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

                function backToNormal(){
                    main_column.state = "normal"
                    root.showAddDeleteButton()
                    add_user_dialog.reset()
                    root.scrollToLastPosition()
                }

                onCancelled: {
                    backToNormal()
                }

                onConfirmed: {
                    dbus_accounts.CreateUser(userInfo.userName, userInfo.userName, userInfo.userAccountType)
                    userCreatingItem.setCreatingUser(userInfo)
                    inUserCreating = true
                    backToNormal()
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
                    guest_user.height = 0
                    hideAddDeleteButton()
                }

                onShowAllPrivate: {
                    guest_user.visible = true
                    guest_user.height = guest_user.childrenRect.height
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

            UserListCreatingItem{
                id: userCreatingItem
                width: parent.width
                visible: inUserCreating
                Connections {
                    target: user_list
                    onUserHasAdded: {
                        if(userCreatingItem.currentUserName == userInfo.userName && userCreatingItem.creatingUserInfo){
                            inUserCreating = false
                            dbus_user.path = userInfo.userDBusPath
                            dbus_user.SetPassword(userCreatingItem.creatingUserInfo.userPassword)
                            dbus_user.SetIconFile(userCreatingItem.creatingUserInfo.userIconFile)
                            dbus_user.SetAccountType(userCreatingItem.creatingUserInfo.userAccountType)
                            dbus_user.SetAutomaticLogin(userCreatingItem.creatingUserInfo.userAutoLogin)
                        }
                    }
                }
            }
        }

        GuestUser {
            id: guest_user
            width: parent.width

            onExpandedChanged: {
                user_list.visible = !expanded

                //make user list button showed
                if (expanded)
                {
                    add_check_button.visible = false
                    delete_check_button.visible = false
                }
                else{
                    add_check_button.visible = true
                    delete_check_button.visible = dbus_accounts.userList.length != 1
                }

                // prevent guest_user from flicking away while showing
                expanded ? root.scrollToTop() : root.scrollToLastPosition()
            }
            anchors.top: main_column.bottom

            leftPadding: user_list.leftPadding
            rightPadding: user_list.rightPadding
            nameLeftPadding: user_list.nameLeftPadding
        }
    }
}
