import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
    id: root_view
    state: "default"

    property int verticalPadding: 8
    property var this_user: null

    signal avatarSet (url path)
    signal avatarPictured (url path)

    DRadioButton {
        id: radio_button

        buttonModel: [
            {"buttonId": "recently_used", "buttonLabel": "Recently Used"},
            {"buttonId": "default", "buttonLabel": "Default"},
            {"buttonId": "webcam", "buttonLabel": "Webcam"},
        ]

        initializeIndex: 1

        onItemSelected: {
            switch (idx) {
                case 0: root_view.state = "recently"; break
                case 1: root_view.state = "default"; break
                case 2: root_view.state = "webcam"; break
            }
        }

        anchors.top: parent.top
        anchors.topMargin: root_view.verticalPadding
        anchors.horizontalCenter: parent.horizontalCenter
    }

    states: [
        State {
            name: "recently"
            PropertyChanges {
                target: root_view
                height: root_view.verticalPadding * 3 + radio_button.height + avatar_recently_used_view.height
            }
            PropertyChanges {
                target: avatar_recently_used_view
                x: 10
                visible: true
            }
            PropertyChanges {
                target: avatar_default_view
                x: root_view.width
                visible: false
            }
            PropertyChanges {
                target: webcam_loader
                x: root_view.width * 2
                sourceComponent: null
                visible: false
            }
        },
        State {
            name: "default"
            PropertyChanges {
                target: root_view
                height: root_view.verticalPadding * 3 + radio_button.height + avatar_default_view.height
            }
            PropertyChanges {
                target: avatar_recently_used_view
                x: - root_view.width
                visible: false
            }
            PropertyChanges {
                target: avatar_default_view
                x: 10
                visible: true
            }
            PropertyChanges {
                target: webcam_loader
                x: root_view.width
                sourceComponent: null
                visible: false
            }
        },
        State {
            name: "webcam"
            PropertyChanges {
                target: root_view
                height: root_view.verticalPadding * 3 + radio_button.height + webcam_loader.height
            }
            PropertyChanges {
                target: avatar_recently_used_view
                x: - root_view.width * 2
                visible: false
            }
            PropertyChanges {
                target: avatar_default_view
                x: - root_view.width
                visible: false
            }
            PropertyChanges {
                target: webcam_loader
                x: 0
                sourceComponent: avatar_webcam_view
                visible: true
            }
        }
    ]

    AvatarIconView {
        id: avatar_recently_used_view
        currentIndex: -1
        itemCanbeDeleted: true

        function setContent() {
            avatar_recently_used_view.model.clear()
            var allIcons = root_view.this_user.historyIcons
            for (var i = 0; i < allIcons.length; i++) {
                avatar_recently_used_view.model.append({"avatarPath": allIcons[i]})
                if (this_user.iconFile == allIcons[i]) {
                    avatar_recently_used_view.currentIndex = i
                }
            }

            width = 290
            height = cellHeight * Math.min(3, Math.ceil(count /3))
        }

        onInitialize: {
            avatar_recently_used_view.setContent()
        }

        Connections {
            target: dbus_user
            onHistoryIconsChanged: {
                avatar_recently_used_view.setContent()
            }
        }

        anchors.top: radio_button.bottom
        anchors.topMargin: root_view.verticalPadding * 2
    }

    DScrollBar {
        id: recently_used_scrollbar
        flickable: avatar_recently_used_view
    }

    AvatarIconView {
        id: avatar_default_view
        currentIndex: -1

        withAddButton: true

        function setContent() {
            avatar_recently_used_view.model.clear()
            var allIcons = root_view.this_user.iconList
            for (var i = 0; i < allIcons.length; i++) {
                avatar_default_view.model.append({"avatarPath": allIcons[i]})
                if (this_user.iconFile == allIcons[i]) {
                    avatar_recently_used_view.currentIndex = i
                }
            }
        }

        onInitialize: setContent()

        anchors.top: radio_button.bottom
        anchors.topMargin: root_view.verticalPadding * 2
    }

    DScrollBar {
        id: default_scrollbar
        flickable: avatar_default_view
        anchors.right: parent.right
        anchors.rightMargin: 5
    }

    Loader {
        id: webcam_loader
        width: 310
        height: 200

        anchors.top: radio_button.bottom
        anchors.topMargin: root_view.verticalPadding * 2
    }

    Component {
        id: avatar_webcam_view

        WebCam {
            running: rootWindow.displayWidth != 0
        }
    }
}