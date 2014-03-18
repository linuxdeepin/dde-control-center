import QtQuick 2.1
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Accounts 1.0

ListView {
    id: root
    interactive: false

    property int leftPadding: 15
    property int rightPadding: 15
    property int nameLeftPadding: 120

    property bool fromPanelAvatar

    signal hideAllPrivate (int idx)
    signal showAllPrivate ()
    signal allNormal ()
    signal allAction ()

    function showCurrentUserDetail () {
        fromPanelAvatar = true
    }

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
            state: root.fromPanelAvatar ? index == 0 ? "edit_dialog" : "invisible" : "normal"

            width: 310
            height: delete_line.height + component_sep.height
            property var this_user: User { path: userDBusPath}

            Connections {
                target: component_bg.ListView.view

                onHideAllPrivate: {
                    if (idx != index) {
                        component_bg.state = "invisible"
                    }
                }
                onShowAllPrivate: {
                    if (component_bg.height == 0) {
                        component_bg.state = "normal"
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
                
                state: "normal"

                property bool deleteUserDialogVisible
                property bool editUserDialogVisible
                property bool nameColumnVisible
                property bool expandButtonVisible
                property bool expandButtonUp

                property color nameColor
                property string expandButtonStatus

                onAction: {
                    component_bg.state = "delete_dialog"
                    delete_line.shrink()
                }

                states: [
                    State {
                        name: "hover_shrinked"
                        PropertyChanges {
                            target: delete_line
                            nameColor: index == 0 ? "#faca57" : dconstants.hoverColor
                            expandButtonStatus: index == 0 ? "golden" : "hover"
                            expandButtonUp: false
                        }
                    },
                    State {
                        name: "hover_expanded"
                        PropertyChanges {
                            target: delete_line
                            nameColor: index == 0 ? "#faca57" : dconstants.hoverColor                            
                            expandButtonStatus: index == 0 ? "golden" : "hover"                            
                            expandButtonUp: true                            
                        }
                    },                    
                    State {
                        name: "normal"
                        PropertyChanges {
                            target: delete_line
                            nameColor: index == 0 ? "#faca57" : dconstants.fgColor
                            expandButtonStatus: index == 0 ? "golden" : "normal"    
                            expandButtonUp: false
                        }
                    }
                ]

                content: Item {
                    id: component_top
                    width: delete_line.width
                    height: delete_line.height

                    DRoundImage {
                        id: round_image
                        roundRadius: index == 0 ? 30 : 25 
                        imageSource: this_user.iconFile

                        anchors.left: parent.left
                        anchors.leftMargin: root.leftPadding + 40 - roundRadius
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Column {
                        id: name_column
                        spacing: 2
                        visible: delete_line.nameColumnVisible

                        DssH2 {
                            text: userName
                            color: delete_line.nameColor
                            font.pixelSize: 14
                        }

                        DssH3 {
                            text: userType == 0 ? dsTr("User") : dsTr("Administrator")
                            color: dconstants.fgColor
                            font.pixelSize: 10                            
                        }

                        anchors.left: parent.left
                        anchors.leftMargin: root.nameLeftPadding
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
                                root.hideAllPrivate(index)
                            } else {
                                component_bg.state = "normal"
                                root.showAllPrivate()
                            }
                        }

                        onEntered: {
                            if (component_bg.state == "normal") {
                                delete_line.state = "hover_shrinked"
                            }
                        }

                        onExited: {
                            if (component_bg.state != "edit_dialog")
                            {
                                delete_line.state = "normal"
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
                        state: "normal"
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
                    name: "invisible"
                    PropertyChanges {
                        target: delete_line
                        state: "normal"                        
                        deleteUserDialogVisible: false
                        editUserDialogVisible: false
                        nameColumnVisible: true
                        expandButtonVisible: true
                    }
                    PropertyChanges {
                        target: component_bg
                        height: 0
                        visible: false
                    }
                },
                State {
                    name: "action"
                    PropertyChanges {
                        target: delete_line
                        state: "normal"                        
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
                        state: "normal"                        
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
                        state: "hover_expanded"                        
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
                SmoothedAnimation { duration: 300 }
            }

            Component.onCompleted: {
                if (index == 0) {root.width = width}
                root.height += height
            }
        }
    }

    model: ListModel {
        id: user_list_model

        Component.onCompleted: {
            var cached_users = dbus_accounts.userList
            for (var i = 0; i < cached_users.length; i++) {
                root.addUser(cached_users[i])
            }
        }
    }

    delegate: delegate_component
}