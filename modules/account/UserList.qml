import QtQuick 2.1
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Accounts 1.0

ListView {
    id: root
    interactive: false

    property int leftPadding: 15
    property int rightPadding: 15
    property int avatarNamePadding: 30

    signal hideAllPrivate (int idx)
    signal showAllPrivate ()
    signal allNormal ()
    signal allAction ()
    signal currentUserDetail ()

    function addUser(path) {
        dbus_user.path = path

        var user_status = dbus_user.locked ? "inactiveUser" : dbus_user.loginTime != 0 ? "currentUser" : "otherUser"
        user_list_model.append({"userAvatar": dbus_user.iconFile,
                                "userId": dbus_user.uid,
                                "userName": dbus_user.userName,
                                "userType": dbus_user.accountType,
                                "userStatus": user_status,
                                "userDBusPath": path})
    }

    function deleteItem (idx) {
        root.model.remove(idx, 1)
    }

    Component {
        id: delegate_component

        Rectangle{
            id: component_bg
            color: "#1A1B1B"
            state: "normal"

            width: 310
            height: delete_line.height + component_sep.height
            property var this_user: User { path: userDBusPath}

            Connections {
                target: component_bg.ListView.view

                onHideAllPrivate: {
                    if (idx != index) {
                        component_bg.height = 0
                        component_bg.visible = false
                    }
                }
                onShowAllPrivate: {
                    if (component_bg.height == 0) {
                        component_bg.height = 100 + 2
                        component_bg.visible = true                        
                    }
                }
                
                onCurrentUserDetail: {
                    if (index == 0) {
                        component_bg.height = 100 + 1
                        component_bg.visible = true
                    } else {
                        component_bg.height = 0
                        component_bg.visible = false
                    }
                }

                onAllAction: {
                    component_bg.state = "action"
                    delete_line.expand()
                }

                onAllNormal: {
                    component_bg.state = "normal"
                    delete_line.shrink()
                }
            }

            DeleteLine {
                id: delete_line
                width: component_bg.width
                height: 100
                moveDelta: 50

                property bool deleteUserDialogVisible
                property bool editUserDialogVisible
                property bool nameColumnVisible
                property bool expandButtonVisible
                property bool expandButtonUp: false
                property string expandButtonStatus: "normal"

                property int roundImageRadius: 25
                property color nameColor: "white"

                onAction: {
                    component_bg.state = "delete_dialog"
                    delete_line.shrink()
                }

                content: Item {
                    id: component_top
                    width: delete_line.width
                    height: delete_line.height

                    DRoundImage {
                        id: round_image
                        roundRadius: delete_line.roundImageRadius
                        imageSource: userAvatar

                        anchors.left: parent.left
                        anchors.leftMargin: root.leftPadding
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Column {
                        id: name_column
                        visible: delete_line.nameColumnVisible

                        DssH2 {
                            text: userName
                            color: delete_line.nameColor
                        }

                        DssH3 {
                            text: userType == 0 ? dsTr("User") : dsTr("Administrator")
                        }

                        anchors.left: round_image.right
                        anchors.leftMargin: root.avatarNamePadding
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    ExpandButton {
                        id: expand_button
                        up: delete_line.expandButtonUp
                        visible: delete_line.expandButtonVisible
                        status: delete_line.expandButtonStatus

                        anchors.right: parent.right
                        anchors.rightMargin: root.rightPadding
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    DeleteUserDialog {
                        id: delete_user_dialog
                        z: 2    // get mouse click action before the mouse area
                        visible: delete_line.deleteUserDialogVisible

                        onCancel: {
                            component_bg.state = "action"
                            delete_line.expand()
                        }
                        onConfirm: {
                            dbus_accounts.DeleteUser(userName, deleteFiles)
                            component_bg.state = "normal"
                            root.deleteItem(index)
                        }

                        anchors.left: round_image.right
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            if (component_bg.state == "normal") {
                                component_bg.state = "edit_dialog"
                                delete_line.expandButtonUp = true
                                root.hideAllPrivate(index)
                            } else {
                                component_bg.state = "normal"
                                delete_line.expandButtonUp = false
                                root.showAllPrivate()
                            }
                        }

                        onEntered: {
                            if (component_bg.state == "normal") {
                                delete_line.roundImageRadius = 30
                                delete_line.nameColor = "#faca57"
                                delete_line.expandButtonStatus = "hover"
                            }
                        }

                        onExited: {
                            if (component_bg.state != "edit_dialog")
                            {
                                delete_line.roundImageRadius = 25
                                delete_line.nameColor = "white"
                                delete_line.expandButtonStatus = "normal"
                            }
                        }
                    }
                }
            }

            DSeparatorHorizontal {
                id: component_sep
                anchors.top: delete_line.bottom
            }

            EditUserDialog {
                id: edit_user_dialog
                visible: delete_line.editUserDialogVisible
                this_user: User { path: userDBusPath}
                anchors.top: component_sep.bottom
            }

            states: [
                State {
                    name: "normal"
                    PropertyChanges {
                        target: delete_line
                        deleteUserDialogVisible: false
                        editUserDialogVisible: false
                        nameColumnVisible: true
                        expandButtonVisible: true
                    }
                    PropertyChanges {
                        target: component_bg
                        height: delete_line.height + component_sep.height
                    }
                },
                State {
                    name: "action"
                    PropertyChanges {
                        target: delete_line
                        deleteUserDialogVisible: false
                        editUserDialogVisible: false
                        nameColumnVisible: true
                        expandButtonVisible: true
                    }
                    PropertyChanges {
                        target: component_bg
                        height: delete_line.height + component_sep.height
                    }
                },
                State {
                    name: "delete_dialog"
                    PropertyChanges {
                        target: delete_line
                        deleteUserDialogVisible: true
                        editUserDialogVisible: false
                        nameColumnVisible: false
                        expandButtonVisible: false
                    }
                    PropertyChanges {
                        target: component_bg
                        height: delete_line.height + component_sep.height
                    }
                },
                State {
                    name: "edit_dialog"
                    PropertyChanges {
                        target: delete_line
                        deleteUserDialogVisible: false
                        editUserDialogVisible: true
                        nameColumnVisible: true
                        expandButtonVisible: true
                    }
                    PropertyChanges {
                        target: component_bg
                        height: delete_line.height + component_sep.height + edit_user_dialog.height
                    }
                }
            ]

            Behavior on height {
                SmoothedAnimation { duration: 50 }
            }

            Component.onCompleted: {
                if (index == 0) {root.width = width}
                root.height += height
            }
        }
    }

    model: ListModel { id: user_list_model }

    delegate: delegate_component

    Component.onCompleted: {
        var cached_users = dbus_accounts.userList
        for (var i = 0; i < cached_users.length; i++) {
            root.addUser(cached_users[i])
        }
    }
}