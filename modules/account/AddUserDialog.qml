import QtQuick 2.1
import "../widgets"

Rectangle {
    id: root

    color: "#1A1B1B"
    width: 310
    height: column.height

    property int leftPadding: 15
    property int rightPadding: 15
    property int avatarNamePadding: 30
    
    signal confirmed (variant userInfo)
    signal cancelled

    Column {
        id: column
        width: 310

        function validate() {
            var result = true
            
            /* user name validation */
            var linux_username_regex = /^[a-z_][a-z0-9_-]{0,30}$/
            if (!linux_username_regex.test(user_name_input.text)) {
                result = false
                user_name_input.state = "warning"
            }
            
            if (password_input.text != repeat_password_input.text) {
                result = false
                password_input.state = "warning"
                repeat_password_input.state = "warning"
            }
            
            return result
        }

        DBaseLine {
            leftLoader.sourceComponent: DssH1 {
                text: "Add User"
                color: "#19A9F9"
            }
        }

        DSeparatorHorizontal {}

        Rectangle {
            width: parent.width
            height: 100
            color: Qt.rgba(0, 0, 0, 0)

            DRoundImage {
                id: round_image
                roundRadius: 25
                imageSource: "/var/lib/AccountsService/icons/guest.jpg"

                property bool toggleFlag: false

                anchors.left: parent.left
                anchors.leftMargin: root.leftPadding
                anchors.verticalCenter: parent.verticalCenter
            }

            Column {
                id: name_column

                DssH2 {
                    text: user_name_input.text || "new user"
                }

                DssH3 {
                    text: ["User", "Administrator"][user_group_radio.currentIndex]
                }

                anchors.left: round_image.right
                anchors.leftMargin: root.avatarNamePadding
                anchors.verticalCenter: parent.verticalCenter
            }

            UserStatusButton {
                id: user_status_button
                state: "inactiveUser"

                anchors.right: parent.right
                anchors.rightMargin: root.rightPadding
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        DSeparatorHorizontal {}

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DLabel {
                text: "User name"
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DTextInput {
                id: user_name_input
                
                KeyNavigation.tab: password_input

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
                text: "Password"
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DTextInput {
                id: password_input
                echoMode: TextInput.Password
                
                KeyNavigation.tab: repeat_password_input

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
                text: "Repeat Password"
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DTextInput {
                id: repeat_password_input
                echoMode: TextInput.Password

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
                text: "Show Password"
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DSwitchButton {
                onClicked: {
                    if(checked) {
                        password_input.echoMode = TextInput.Normal
                        repeat_password_input.echoMode = TextInput.Normal
                    } else {
                        password_input.echoMode = TextInput.Password
                        repeat_password_input.echoMode = TextInput.Password
                    }
                }

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        DSeparatorHorizontal{}

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DLabel {
                text: "User Group"
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DRadioButton {
                id: user_group_radio

                buttonModel: [
                    {"buttonId": "user", "buttonLabel": "User"},
                    {"buttonId": "administrator", "buttonLabel": "Administrator"},
                ]
                
                initializeIndex: 0
                
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
                text: "Auto-login"
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DSwitchButton {
                id: auto_login_switch
                
                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        DSeparatorHorizontal{}

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DLabel {
                text: "Face Recognition"
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DSwitchButton {
                id: face_recognition_switch
                
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
                text: "Cancel"

                anchors.right: confirm_button.left
                anchors.rightMargin: 5
                anchors.verticalCenter: parent.verticalCenter

                onClicked: {
                    root.cancelled()
                }
            }

            DTextButton {
                id: confirm_button
                text: "Confirm"

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter

                onClicked: {
                    if (column.validate()) {
                        root.confirmed({userName: user_name_input.text,
                                        userPassword: password_input.text,
                                        userAccountType: user_group_radio.currentIndex,
                                        userAutoLogin: auto_login_switch.checked,
                                        userFaceRecognition: face_recognition_switch.checked})
                    }
                }
            }
        }
        
        DSeparatorHorizontal {}
    }
}
