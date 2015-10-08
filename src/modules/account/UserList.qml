import QtQuick 2.1
import QtGraphicalEffects 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Accounts 1.0

ListView {
    id: root
    interactive: false

    width: 310
    height: childrenRect.height

    property int leftPadding: 15
    property int rightPadding: 15
    property int nameLeftPadding: 120

    property bool fromPanelAvatar

    signal hideAllPrivate (int idx)
    signal showAllPrivate ()
    signal allNormal ()
    signal allAction ()
    signal actionButtonClicked ()

    signal userHasAdded(var userInfo)

    function showCurrentUserDetail () {
        fromPanelAvatar = true
    }

    function backToNormal(){
        for (var i = 0; i < root.children[0].children.length; i++) {
            if (root.children[0].children[i].state !== ""){
                root.children[0].children[i].state = "normal"
            }
        }
    }

    function addUser(path) {
        dbus_user.path = path

        var user_status = dbus_user.locked ? "inactiveUser" : userIsCurrentUser(dbus_user) ? "currentUser" : "otherUser"
        var user_dict = {"userAvatar": dbus_user.iconFile,
                         "userId": dbus_user.uid,
                         "userName": dbus_user.userName,
                         "userType": dbus_user.accountType,
                         "userStatus": user_status,
                         "userDBusPath": path,
                         "avatarErrMsgShowed":false}
        user_status == "currentUser" ? user_list_model.insert(0, user_dict) : user_list_model.append(user_dict)
        root.userHasAdded(user_dict)
    }

    function deleteUser(path) {
        for (var i = 0; i < user_list_model.count; i++) {
            if (user_list_model.get(i).userDBusPath == path) {
                user_list_model.remove(i, 1)
            }
        }
    }

    function showChooseAvatarIconTypeErrMsg(uid,flag){
        for (var i = 0; i < user_list_model.count; i++) {
            if (user_list_model.get(i).userId == uid) {
                user_list_model.setProperty(i,"avatarErrMsgShowed",flag)
                if (flag){
                    hideAvatarErrMsgTimer.stop()
                    hideAvatarErrMsgTimer.uid = uid
                    hideAvatarErrMsgTimer.start()
                }
            }
        }
    }

    Timer {
        id:hideAvatarErrMsgTimer
        interval: 2000
        repeat: false

        property string uid:""

        onTriggered: {
            showChooseAvatarIconTypeErrMsg(uid,false)
        }
    }

    Component {
        id: delegate_component

        Rectangle{
            id: component_bg
            color: "#1A1B1B"
            state: root.fromPanelAvatar ? userIsCurrentUser(this_user) ? "edit_dialog" : "invisible" : "normal"

            width: 310
            height: delete_line.height + component_sep.height
            property var this_user: User { path: userDBusPath}

            Timer {
                id: to_edit_timer
                interval: 200
                onTriggered: {
                    component_bg.state = "edit_dialog"
                }
            }

            Connections {
                target: component_bg.ListView.view

                onHideAllPrivate: {
                    if (idx != index) {
                        component_bg.state = "invisible"
                    }
                    else {
                        to_edit_timer.restart()
                    }
                }
                onShowAllPrivate: {
                    if (component_bg.height == 0) {
                        component_bg.state = "normal"
                    }
                    editUserDialogLoader.item.reset()
                }

                onAllAction: {
                    if (!userIsCurrentUser(this_user)) {
                        component_bg.state = "action"
                        delete_line.expand()
                    }
                }

                onAllNormal: {
                    component_bg.state = "normal"
                    delete_line.shrink()
                }
            }

            DeleteLine {
                id: delete_line
                width: component_bg.width
                moveDelta: 50

                state: "normal"

                property bool deleteUserDialogVisible
                property bool editUserDialogVisible
                property bool nameColumnVisible
                property bool expandButtonVisible
                property bool expandButtonUp

                property color nameColor
                property string expandButtonStatus: "normal"

                Component.onCompleted: height = userIsCurrentUser(this_user) ? 116 : 100

                onAction: {
                    component_bg.state = "delete_dialog"
                    delete_line.shrink()
                    root.actionButtonClicked()
                }

                states: [
                    State {
                        name: "hover_shrinked"
                        PropertyChanges {
                            target: delete_line
                            nameColor: userIsCurrentUser(this_user) ? "#faca57" : DPalette.hoverColor
                            expandButtonStatus: userIsCurrentUser(this_user) ? "golden" : "hover"
                            expandButtonUp: false
                        }
                    },
                    State {
                        name: "hover_expanded"
                        PropertyChanges {
                            target: delete_line
                            nameColor: userIsCurrentUser(this_user) ? "#faca57" : DPalette.hoverColor
                            expandButtonStatus: userIsCurrentUser(this_user) ? "golden" : "hover"
                            expandButtonUp: true
                        }
                    },
                    State {
                        name: "normal"
                        PropertyChanges {
                            target: delete_line
                            nameColor: userIsCurrentUser(this_user) ? "#faca57" : DPalette.fgColor
                            expandButtonStatus: userIsCurrentUser(this_user) ? "golden" : "normal"
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
                        roundRadius: index == 0 ? 36 : 30
                        imageSource: "file://" + this_user.iconFile

                        anchors.left: parent.left
                        anchors.leftMargin: root.leftPadding + 40 - roundRadius
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Desaturate {
                        anchors.fill: round_image
                        source: round_image
                        desaturation: 1.0
                        visible: this_user.locked
                    }

                    Column {
                        id: name_column
                        spacing: 2
                        visible: delete_line.nameColumnVisible

                        DssH2 {
                            text: this_user.userName
                            color: delete_line.nameColor
                            width: 140
                            elide: Text.ElideRight
                            font.pixelSize: 14
                        }

                        DssH3 {
                            text: this_user.accountType == 0 ? dsTr("Normal User") : dsTr("Administrator")
                            color: DPalette.fgColor
                            font.pixelSize: 10
                        }

                        anchors.left: parent.left
                        anchors.leftMargin: root.nameLeftPadding
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    DssH2 {
                        text: dsTr("Image format not supported")
                        color: "#FF8C03"
                        font.pixelSize: 14
                        visible: avatarErrMsgShowed
                        anchors.left:name_column.left
                        anchors.top:name_column.bottom
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
                            root.allNormal()
                        }
                        onConfirm: {
                            var right = dbus_accounts.DeleteUser(userName, deleteFiles)
                            root.allNormal()
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
                            //data is undefined,do nothing
                            if (typeof(userName) === "undefined"){
                                return
                            }

                            if (component_bg.state == "normal") {
                                /* component_bg.state = "edit_dialog" */
                                root.hideAllPrivate(index)
                            } else if (delete_line.state !== "normal") {
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


            Loader {//In order to load list quickly,Large resource load leater
                id:editUserDialogLoader
                active: false
                anchors.top: component_sep.bottom
                sourceComponent: EditUserDialog {
                    id: edit_user_dialog
                    maxHeight: frame.height - title_column.height - delete_line.height
                    visible: delete_line.editUserDialogVisible
                    this_user: User { path: userDBusPath}
                    onSetAvatarIconError: {
                        showChooseAvatarIconTypeErrMsg(uid,true)
                    }
                }
            }

            Timer {
                running: true
                interval: 300
                onTriggered: editUserDialogLoader.active = true
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
                        height: delete_line.height + component_sep.height + editUserDialogLoader.item.height
                    }
                }
            ]

            Behavior on height {
                SmoothedAnimation { duration: 300 }
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
