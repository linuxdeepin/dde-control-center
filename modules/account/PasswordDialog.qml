import QtQuick 2.1
import "../widgets"

Item {
    id: root
    width: 310
    state: "brief"

    states: [
        State {
            name: "brief"
            PropertyChanges {
                target: root
                height: brief_view.height
            }
            PropertyChanges {
                target: brief_view
                visible: true
            }
            PropertyChanges {
                target: detail_view
                visible: false
            }
        },
        State {
            name: "detail"
            PropertyChanges {
                target: root
                height: detail_view.height
            }
            PropertyChanges {
                target: brief_view
                visible: false
            }
            PropertyChanges {
                target: detail_view
                visible: true
            }
        }
    ]

    Behavior on height {
        SmoothedAnimation {duration: 200}
    }

    Rectangle {
        id: brief_view

        width: parent.width
        height: 38
        color: "transparent"

        DLabel {
            text: "Password"
            font.pixelSize: 12

            anchors.left: parent.left
            anchors.leftMargin: 15
            anchors.verticalCenter: parent.verticalCenter
        }

        DLabel {
            text: "\u2022\u2022\u2022\u2022"
            font.pixelSize: 18

            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    root.state = "detail"
                }
            }
        }
    }

    DColumn {
        id: detail_view
        height: (38 + 2) * 5

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DLabel {
                text: "Current Password"
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DTextInput {
                echoMode: TextInput.Password

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DLabel {
                text: "Current Password"
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DTextInput {
                echoMode: TextInput.Password

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DLabel {
                text: "Current Password"
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DTextInput {
                echoMode: TextInput.Password

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DLabel {
                text: "Show Password"
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DSwitchButton {
                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DTextButton {
                id: cancel_button
                text: "Cancel"

                anchors.right: confirm_button.left
                anchors.rightMargin: 5
                anchors.verticalCenter: parent.verticalCenter

                onClicked: {
                }
            }

            DTextButton {
                id: confirm_button
                text: "Confirm"

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter

                onClicked: {
                }
            }
        }
        
        DSeparatorHorizontal {}

        anchors.right: parent.right
    }
}
