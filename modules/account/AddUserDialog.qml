import QtQuick 2.1
import Deepin.Widgets 1.0

Rectangle {
    id: root

    color: "#1A1B1B"
    width: 310
    height: column.height

    property int leftPadding: 15
    property int rightPadding: 15
    property int avatarNamePadding: 30
    
    signal confirmed (var userInfo)
    signal cancelled
    
    function warnUserName() {
        user_name_input.state = "warning"
    }
    
    function reset() {
        round_image.imageSource = dbus_accounts.RandUserIcon()[0]
        user_name_input.focus = true
        user_name_input.text = ""
        password_input.text = ""
        repeat_password_input.text = ""
        auto_login_switch.checked = false
        user_group_radio.selectItem(0)
    }
    
    Column {
        id: column
        width: 310

        function validate() {
            var result = true
            
            /* user name validation */
            var linux_username_regex = /^[a-z_][a-z0-9_-]{0,30}$/i
            if (!linux_username_regex.test(user_name_input.text)) {
                result = false
                user_name_input.state = "warning"
            }
            
            if (password_input.text == "") {
                result = false
                password_input.state = "warning"
            }
            
            if (repeat_password_input.text == "") {
                result = false
                repeat_password_input.state = "warning"
            }            
            
            if (password_input.text != repeat_password_input.text) {
                result = false
                repeat_password_input.state = "warning"
            }
            
            return result
        }

        DBaseLine {
            leftLoader.sourceComponent: DssH1 {
                text: dsTr("Add User")
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
                imageSource: "file://" + dbus_accounts.RandUserIcon()[0]

                property bool toggleFlag: false

                anchors.left: parent.left
                anchors.leftMargin: root.leftPadding
                anchors.verticalCenter: parent.verticalCenter
            }

            Column {
                id: name_column

                DssH2 {
                    text: user_name_input.text || dsTr("New User")
                }

                DssH3 {
                    text: [dsTr("Standard"), dsTr("Administrator")][user_group_radio.currentIndex]
                }

                anchors.left: round_image.right
                anchors.leftMargin: root.avatarNamePadding
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        DSeparatorHorizontal {}

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DLabel {
                text: dsTr("Username")
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
                onTextChanged: {
                    if(state=="warning"){
                        state = "normal"
                    }
                }
            }
        }

        DSeparatorHorizontal {}

        Rectangle {
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

            DTextInput {
                id: password_input
                echoMode: TextInput.Password
                
                KeyNavigation.tab: repeat_password_input

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                onTextChanged: {
                    if(state=="warning"){
                        state = "normal"
                    }
                }
            }
        }

        DSeparatorHorizontal {}

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DLabel {
                text: dsTr("Repeat Password")
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
                onTextChanged: {
                    if(state=="warning"){
                        state = "normal"
                    }
                }
            }
        }

        DSeparatorHorizontal {}

        Rectangle {
            width: parent.width
            height: 38
            color: "transparent"

            DLabel {
                text: dsTr("Account Type")
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
            }

            DRadioButton {
                id: user_group_radio

                buttonModel: [
                    {"buttonId": "user", "buttonLabel": dsTr("Standard")},
                    {"buttonId": "administrator", "buttonLabel": dsTr("Administrator")},
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
                text: dsTr("Auto-login")
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

        /* DSeparatorHorizontal{} */

        /* Rectangle { */
        /*     width: parent.width */
        /*     height: 38 */
        /*     color: "transparent" */

        /*     DLabel { */
        /*         text: "Face Recognition" */
        /*         font.pixelSize: 12 */

        /*         anchors.left: parent.left */
        /*         anchors.leftMargin: 15 */
        /*         anchors.verticalCenter: parent.verticalCenter */
        /*     } */

        /*     DSwitchButton { */
        /*         id: face_recognition_switch */
                
        /*         anchors.right: parent.right */
        /*         anchors.rightMargin: 15 */
        /*         anchors.verticalCenter: parent.verticalCenter */
        /*     } */
        /* } */

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
                }
            }

            DTextButton {
                id: confirm_button
                text: dsTr("Confirm")

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter

                onClicked: {
                    focus = true
                    if (column.validate()) {
                        root.confirmed({userName: user_name_input.text,
                                        userPassword: password_input.text,
                                        userAccountType: user_group_radio.currentIndex,
                                        userAutoLogin: auto_login_switch.checked,
                                        userIconFile: round_image.imageSource.toString().substring(7),
                                        /* userFaceRecognition: face_recognition_switch.checked */ })
                    }
                }
            }
        }
        
        DSeparatorHorizontal {}
    }
}
