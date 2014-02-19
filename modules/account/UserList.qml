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
                    }
                }
                onShowAllPrivate: {
                    if (component_bg.height == 0) {
                        component_bg.height = 100 + 2
                    }
                }

                onAllAction: {
                    component_bg.state = "action"
                }

                onAllNormal: {
                    component_bg.state = "normal"
                }
            }

            DeleteLine {
                id: delete_line
                width: component_bg.width
                height: 100

                property bool deleteUserDialogVisible
                property bool editUserDialogVisible
                property bool nameColumnVisible
                property bool expandButtonVisible
                property string expandButtonState: "normal"

                property int roundImageRadius: 25
                property color nameColor: "white"

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        if (component_bg.state == "normal") {
                            component_bg.state = "edit_dialog"
                            root.hideAllPrivate(index)
                        } else {
                            component_bg.state = "normal"
                            root.showAllPrivate()
                        }
                    }

                    onEntered: {
                        delete_line.roundImageRadius = 30
                        delete_line.nameColor = "#faca57"
                        delete_line.expandButtonState = "hover"
                    }

                    onExited: {
                        if (component_bg.state != "edit_dialog") {
                            delete_line.roundImageRadius = 25
                            delete_line.nameColor = "white"
                            delete_line.expandButtonState = "normal"
                        }
                    }
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
                        state: delete_line.expandButtonState
                        visible: delete_line.expandButtonVisible

                        anchors.right: parent.right
                        anchors.rightMargin: root.rightPadding
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    DeleteUserDialog {
                        id: delete_user_dialog
                        visible: delete_line.deleteUserDialogVisible

                        onCancel: {
                            component_bg.state = "action"

                        }
                        onConfirm: {
                            dbus_accounts.DeleteUser(userId, deleteFiles)
                            component_bg.state = "normal"
                            root.deleteItem(index)
                        }

                        anchors.left: round_image.right
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
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
                        expandButtonVisible: false
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
                        height: delete_line.height + component_sep.height + delete_user_dialog.height
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
                SmoothedAnimation { duration: 200 }
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
        var cached_users = dbus_accounts.ListCachedUsers()
        for (var i = 0; i < cached_users.length; i++) {
            root.addUser(cached_users[i])
        }
    }
}