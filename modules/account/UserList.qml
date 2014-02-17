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
            height: component_top.height + component_sep.height
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

            Rectangle {
                id: component_top

                height: 100
                color: Qt.rgba(0, 0, 0, 0)

                DeleteUserButton {
                    id: delete_user_button

                    onClicked: {
                        component_bg.state = "delete_dialog"
                    }

                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                }

                DRoundImage {
                    id: round_image
                    roundRadius: 25
                    imageSource: userAvatar

                    property bool toggleFlag: false

                    onClicked: {
                        if (!toggleFlag) {
                            component_bg.state = "edit_dialog"
                            component_bg.ListView.view.hideAllPrivate(index)
                        } else {
                            component_bg.state = "normal"
                            component_bg.ListView.view.showAllPrivate()
                        }
                        toggleFlag = !toggleFlag
                    }

                    anchors.left: delete_user_button.right
                    anchors.leftMargin: root.leftPadding
                    anchors.verticalCenter: parent.verticalCenter
                }

                Column {
                    id: name_column

                    DssH2 {
                        text: userName
                    }

                    DssH3 {
                        text: userType == 0 ? dsTr("User") : dsTr("Administrator")
                    }

                    anchors.left: round_image.right
                    anchors.leftMargin: root.avatarNamePadding
                    anchors.verticalCenter: parent.verticalCenter
                }

                UserStatusButton {
                    id: user_status_button
                    state: userStatus

                    onChangeStatus: {
                        component_bg.this_user.locked = locked
                    }

                    anchors.right: parent.right
                    anchors.rightMargin: root.rightPadding
                    anchors.verticalCenter: parent.verticalCenter
                }

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: root.leftPadding
                anchors.rightMargin: root.rightPadding
            }

            DSeparatorHorizontal {
                id: component_sep

                anchors.top: component_top.bottom
            }

            DeleteUserDialog {
                id: delete_user_dialog
                width: 310
                height: 100
                visible: false

                onCancel: {
                    component_bg.state = "action"

                }
                onConfirm: {
                    dbus_accounts.DeleteUser(userId, deleteFiles)
                    component_bg.state = "normal"
                    root.deleteItem(index)
                }

                anchors.top: component_sep.bottom
            }

            EditUserDialog {
                id: edit_user_dialog

                this_user: User { path: userDBusPath}

                ParallelAnimation {
                    id: animation

                    property var destination: round_image.parent.mapToItem(component_bg, round_image.x + 15, round_image.y)

                    property Item target: round_image
                    property point startPoint: Qt.point(target.x, target.y)
                    property point endPoint: Qt.point(destination.x, destination.y)
                    property int startRoundRadius: target.roundRadius
                    property int endRoundRadius: round_image.roundRadius

                    NumberAnimation {
                        target: animation.target
                        duration: 500
                        easing.type: Easing.InQuad

                        properties: "roundRadius"
                        from: animation.startRoundRadius
                        to: animation.endRoundRadius
                    }

                    PathAnimation {
                        target: animation.target
                        duration: 500
                        easing.type: Easing.InQuad

                        path: Path {
                            startX: animation.startPoint.x
                            startY: animation.startPoint.y

                            PathCubic {
                                x: animation.endPoint.x; y: animation.endPoint.y
                                relativeControl1X: -10; relativeControl1Y: -30
                                relativeControl2X: 10; relativeControl2Y: -20
                            }
                        }
                    }

                    onStopped: {
                        round_image.imageSource = target.imageSource
                        target.destroy()
                    }
                }

                onAvatarSet: {
                    var newObject = Qt.createQmlObject('import QtQuick 2.1; import Deepin.Widgets 1.0; DRoundImage {}',
                                                       component_bg, "new");
                    var startPoint = item.parent.mapToItem(component_bg, item.x, item.y)

                    newObject.x = startPoint.x
                    newObject.y = startPoint.y
                    newObject.imageSource = item.imageSource
                    newObject.roundRadius = item.roundRadius

                    animation.target = newObject
                    animation.start()
                }

                onAvatarPictured: {
                    print("onAvatarPictured, ", path)
                    var newObject = Qt.createQmlObject('import QtQuick 2.1; import Deepin.Widgets 1.0; DRoundImage {}',
                                                       component_bg, "new");
                    var startPoint = item.parent.mapToItem(component_bg, item.x, item.y)

                    newObject.x = startPoint.x
                    newObject.y = startPoint.y
                    newObject.imageSource = path
                    newObject.roundRadius = 60

                    animation.target = newObject
                    animation.start()
                }

                anchors.top: component_sep.bottom
            }

            states: [
                State {
                    name: "normal"
                    PropertyChanges {
                        target: delete_user_dialog
                        visible: false
                    }
                    PropertyChanges {
                        target: edit_user_dialog
                        visible: false
                    }
                    PropertyChanges {
                        target: delete_user_button
                        visible: false
                    }
                    PropertyChanges {
                        target: component_bg
                        height: component_top.height + component_sep.height
                    }
                    PropertyChanges {
                        target: user_status_button
                        visible: true
                    }
                },
                State {
                    name: "action"
                    PropertyChanges {
                        target: delete_user_dialog
                        visible: false
                    }
                    PropertyChanges {
                        target: edit_user_dialog
                        visible: false
                    }
                    PropertyChanges {
                        target: user_status_button
                        visible: false
                    }
                    PropertyChanges {
                        target: component_bg
                        height: component_top.height + component_sep.height
                    }
                    PropertyChanges {
                        target: delete_user_button
                        visible: true
                    }
                },
                State {
                    name: "delete_dialog"
                    PropertyChanges {
                        target: edit_user_dialog
                        visible: false
                    }
                    PropertyChanges {
                        target: user_status_button
                        visible: false
                    }
                    PropertyChanges {
                        target: component_bg
                        height: component_top.height + component_sep.height + delete_user_dialog.height
                    }
                    PropertyChanges {
                        target: delete_user_button
                        visible: true
                    }
                    PropertyChanges {
                        target: delete_user_dialog
                        visible: true
                    }
                },
                State {
                    name: "edit_dialog"
                    PropertyChanges {
                        target: delete_user_dialog
                        visible: false
                    }
                    PropertyChanges {
                        target: user_status_button
                        visible: false
                    }
                    PropertyChanges {
                        target: delete_user_button
                        visible: false
                    }
                    PropertyChanges {
                        target: component_bg
                        height: component_top.height + component_sep.height + edit_user_dialog.height
                    }
                    PropertyChanges {
                        target: edit_user_dialog
                        visible: true
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