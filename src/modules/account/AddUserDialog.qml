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
        round_image.imageSource = "file://" + dbus_accounts.RandUserIcon()[0]
        user_name_input.focus = true
        user_name_input.text = ""
        password_input.text = ""
        repeat_password_input.text = ""
        auto_login_switch.checked = false
        user_group_radio.selectItem(0)

        user_name_input.warningState = false
        password_input.warningState = false
        repeat_password_input.warningState = false
    }

    function verifyUserName(){
        var user_name_check_result = dbus_accounts.IsUsernameValid(user_name_input.text.toLowerCase())
        if(!user_name_check_result[0]) {
            user_name_input.showWarning(user_name_check_result[1])
            return user_name_check_result[2]
        }
        else
            return user_name_check_result[2]
    }

    function verifyPassword(){
        if (password_input.text == "" || repeat_password_input.text == "")
            return true
        if (password_input.text != repeat_password_input.text) {
            repeat_password_input.showWarning(dsTr("Different password"))
            return false
        }
        else{
            repeat_password_input.warningState = false
            return true
        }

    }

    Column {
        id: column
        width: 310

        function validate() {
            var user_name_check_result = dbus_accounts.IsUsernameValid(user_name_input.text.toLowerCase())
            if(!user_name_check_result[0]) {
                user_name_input.showWarning(user_name_check_result[1])
                return false
            }

            if (password_input.text == "" || !dbus_accounts.IsPasswordValid(password_input.text)) {
                password_input.showWarning(dsTr("Invalid password"))
                return false
            }

            if (repeat_password_input.text == "") {
                repeat_password_input.showWarning(dsTr("Nothing input"))
                return false
            }

            if (password_input.text != repeat_password_input.text) {
                repeat_password_input.showWarning(dsTr("Different password"))
                return false
            }

            return true
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
                    font.capitalization: Font.AllLowercase
                    text: user_name_input.text.toLowerCase() || dsTr("New User")
                }

                DssH3 {
                    text: [dsTr("Normal User"), dsTr("Administrator")][user_group_radio.currentIndex]
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

                property bool warningState: false
                property string oldText: ""

                KeyNavigation.tab: password_input

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                textInput.font.capitalization: Font.AllLowercase
                onTextChanged: {
                    warningState = false
                    if (text != ""){//when confirmed,show empty warning
                        var verifyValue = verifyUserName()
                        if (verifyValue == -1){
                            oldText = text
                        }
                        else{
                            if (verifyValue != 4 && verifyValue != 5)
                                text = oldText
                            warningState = true
                        }
                    }
                    else
                        oldText = ""
                }

                function showWarning(msg) {
                    var input_coord = user_name_input.mapToItem(root, 0, 0)

                    warning_arrow_rect.x = input_coord.x - warning_arrow_rect.shadowWidth
                    warning_arrow_rect.y = input_coord.y + user_name_input.height - 4
                    warning_arrow_rect.warningText = msg

                    warningState = true
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

                property bool warningState: false

                KeyNavigation.tab: repeat_password_input

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                onTextChanged: {
                    warningState = false
                    repeat_password_input.warningState = false
                    checkTimer.stop()
                    checkTimer.start()
                }

                function showWarning(msg) {
                    var input_coord = password_input.mapToItem(root, 0, 0)

                    warning_arrow_rect.x = input_coord.x - warning_arrow_rect.shadowWidth
                    warning_arrow_rect.y = input_coord.y + password_input.height - 4
                    warning_arrow_rect.warningText = msg

                    warningState = true
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

                property bool warningState: false

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                onTextChanged: {
                    warningState = false
                    checkTimer.stop()
                    checkTimer.start()
                }

                function showWarning(msg) {
                    var input_coord = repeat_password_input.mapToItem(root, 0, 0)

                    warning_arrow_rect.x = input_coord.x - warning_arrow_rect.shadowWidth
                    warning_arrow_rect.y = input_coord.y + repeat_password_input.height - 4
                    warning_arrow_rect.warningText = msg

                    warningState = true
                }
            }
        }

        Timer {
            id:checkTimer
            repeat: false
            interval: 700
            running: false
            onTriggered: verifyPassword()
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
                    {"buttonId": "user", "buttonLabel": dsTr("Normal User")},
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
                        root.confirmed({userName: user_name_input.text.toLowerCase(),
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

    WarningArrowRect {
        id: warning_arrow_rect
        visible: user_name_input.warningState || password_input.warningState || repeat_password_input.warningState
        width: user_name_input.width + shadowWidth * 2
    }
}
