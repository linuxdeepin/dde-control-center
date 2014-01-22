import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
    id: root
    state: "default"

    property int verticalPadding: 8
    property var this_user: null

    signal avatarSet (Item item)
    signal avatarPictured (Item item, url path)

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
                case 0: root.state = "recently"; break
                case 1: root.state = "default"; break
                case 2: root.state = "webcam"; break
            }
        }

        anchors.top: parent.top
        anchors.topMargin: root.verticalPadding
        anchors.horizontalCenter: parent.horizontalCenter
    }

    states: [
        State {
            name: "recently"
            PropertyChanges {
                target: root
                height: root.verticalPadding * 3 + radio_button.height + avatar_recently_used_view.height
            }
            PropertyChanges {
                target: avatar_recently_used_view
                x: 0
                visible: true
            }
            PropertyChanges {
                target: avatar_default_view
                x: root.width
                visible: false
            }
            PropertyChanges {
                target: webcam_loader
                x: root.width * 2
                sourceComponent: null
                visible: false
            }
        },
        State {
            name: "default"
            PropertyChanges {
                target: root
                height: root.verticalPadding * 3 + radio_button.height + avatar_default_view.height
            }
            PropertyChanges {
                target: avatar_recently_used_view
                x: - root.width
                visible: false
            }
            PropertyChanges {
                target: avatar_default_view
                x: 0
                visible: true
            }
            PropertyChanges {
                target: webcam_loader
                x: root.width
                sourceComponent: null
                visible: false
            }
        },
        State {
            name: "webcam"
            PropertyChanges {
                target: root
                height: root.verticalPadding * 3 + radio_button.height + webcam_loader.height
            }
            PropertyChanges {
                target: avatar_recently_used_view
                x: - root.width * 2
                visible: false
            }
            PropertyChanges {
                target: avatar_default_view
                x: - root.width
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

        onAvatarSet: {
            root.avatarSet(item)
        }

        onInitialize: {
            var allIcons = root.this_user.AllHistoryIcons()
            for (var i = 0; i < allIcons.length; i++) {
                avatar_recently_used_view.model.append({"avatarPath": allIcons[i]})
            }
        }

        anchors.top: radio_button.bottom
        anchors.topMargin: root.verticalPadding * 2

        Behavior on x {
            SmoothedAnimation { duration: 200 }
        }
    }
    
    DScrollBar {
        id: recently_used_scrollbar
        flickable: avatar_recently_used_view
    }

    AvatarIconView {
        id: avatar_default_view

        withAddButton: true

        onAvatarSet: {
            root.avatarSet(item)
        }
        
        onInitialize: {
            var allIcons = root.this_user.AllAccountsIcons()
            for (var i = 0; i < allIcons.length; i++) {
                avatar_default_view.model.append({"avatarPath": allIcons[i]})
            }
        }

        anchors.top: radio_button.bottom
        anchors.topMargin: root.verticalPadding * 2

        Behavior on x {
            SmoothedAnimation { duration: 200 }
        }
    }
    
    DScrollBar {
        id: default_scrollbar
        flickable: avatar_default_view
    }

    Loader {
        id: webcam_loader
        width: 310
        height: 200

        anchors.top: radio_button.bottom
        anchors.topMargin: root.verticalPadding * 2
    }

    Component {
        id: avatar_webcam_view

        WebCam {
            onAvatarPictured: {
                root.avatarPictured(item, path)
            }

            Behavior on x {
                SmoothedAnimation { duration: 200 }
            }
        }
    }
}