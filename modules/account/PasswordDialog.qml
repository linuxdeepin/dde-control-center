import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
    id: root
    width: 310
    state: "brief"

    signal passwordSet (string password)
    signal cancelled

    function reset() {
        new_password_input.text = ""
        repeat_input.text = ""
        state = "brief"
    }

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

    Rectangle {
        id: brief_view

        width: parent.width
        height: 38
        color: "transparent"

        DLabel {
            text: dsTr("Password")
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

    Column {
        id: detail_view
        width: parent.width
        height: (38 + 2) * 3

        property int echoMode: TextInput.Password

        function validate() {
            return new_password_input.text == repeat_input.text
        }

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DLabel {
                text: dsTr("New Password")
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DTextInput {
                id: new_password_input
                focus: true
                echoMode: detail_view.echoMode

                KeyNavigation.tab: repeat_input

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        DSeparatorHorizontal {}

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DLabel {
                text: dsTr("Repeat")
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DTextInput {
                id: repeat_input
                echoMode: detail_view.echoMode

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        DSeparatorHorizontal {}

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DTextButton {
                id: cancel_button
                text: dsTr("Cancel")

                anchors.right: confirm_button.left
                anchors.rightMargin: 5
                anchors.verticalCenter: parent.verticalCenter

                onClicked: {
                    root.cancelled()
                    root.state = "brief"
                }
            }

            DTextButton {
                id: confirm_button
                text: dsTr("Confirm")

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter

                onClicked: {
                    if (detail_view.validate()) {
                        root.passwordSet(new_password_input.text)
                        root.state = "brief"
                    } else {
                        new_password_input.state = "warning"
                        repeat_input.state = "warning"
                    }
                }
            }
        }

        DSeparatorHorizontal {}

        anchors.right: parent.right
    }
}
