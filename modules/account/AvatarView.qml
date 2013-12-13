import QtQuick 2.1
import "../widgets"

Item {
    id: root
    width: 310
    height: 300
    state: "default"

    property int verticalPadding: 8

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
                target: avatar_webcam_view
                x: root.width * 2
                visible: false
                running: false
            }
        },
        State {
            name: "default"
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
                target: avatar_webcam_view
                x: root.width
                visible: false
                running: false
            }
        },
        State {
            name: "webcam"
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
                target: avatar_webcam_view
                x: 0
                visible: true
                running: true
            }
        }
    ]

    AvatarIconView {
        id: avatar_recently_used_view

        width: parent.width
        height: parent.height - root.verticalPadding * 3 - radio_button.height

        anchors.top: radio_button.bottom
        anchors.topMargin: root.verticalPadding * 2
        
        Behavior on x {
            SmoothedAnimation { duration: 200 }            
        }
    }

    AvatarIconView {
        id: avatar_default_view

        width: parent.width
        height: parent.height - root.verticalPadding * 3 - radio_button.height        
        
        withAddButton: true

        anchors.top: radio_button.bottom
        anchors.topMargin: root.verticalPadding * 2
        
        Behavior on x {
            SmoothedAnimation { duration: 200 }            
        }
    }

    WebCam {
        id: avatar_webcam_view
        width: parent.width
        height: parent.height - root.verticalPadding * 3- radio_button.height        
        
        anchors.top: radio_button.bottom
        anchors.topMargin: root.verticalPadding * 2
        
        Behavior on x {
            SmoothedAnimation { duration: 200 }            
        }
    }
}