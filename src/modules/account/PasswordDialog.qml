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
        
        new_password_input.warningState = false
        repeat_input.warningState = false
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
            if (new_password_input.text == "") {
                new_password_input.showWarning(dsTr("Password can not be empty."))
                return false
            }

            if (repeat_input.text == "") {
                repeat_input.showWarning(dsTr("Password can not be empty."))
                return false
            }

            if (new_password_input.text != repeat_input.text) {
                repeat_input.showWarning(dsTr("The two passwords do not match."))
                return false
            }

            return true
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
                
                property bool warningState: false

                KeyNavigation.tab: repeat_input

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter

                onTextChanged: {
                    warningState = false
                    repeat_input.warningState = false

                    if (repeat_input.text != "" && repeat_input.text != text){
                        repeat_input.warningState = true
                        repeat_input.showWarning(dsTr("The two passwords do not match."))
                    }
                }

                onFocusChanged: {
                    if (focus){
                        warningState = false

                        if (repeat_input.text != "" && repeat_input.text != new_password_input.text){
                            repeat_input.warningState = true
                            repeat_input.showWarning(dsTr("The two passwords do not match."))
                        }
                    }
                    else if(new_password_input.text == "" && warning_arrow_rect != null){
                        warningState = true
                        showWarning(dsTr("Password can not be empty."))
                    }
                }

                function showWarning(msg) {
                    var input_coord = new_password_input.mapToItem(root, 0, 0)
                    
                    warning_arrow_rect.x = input_coord.x - warning_arrow_rect.shadowWidth
                    warning_arrow_rect.y = input_coord.y + new_password_input.height - 4 
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
                text: dsTr("Repeat")
                font.pixelSize: 12

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                onTextChanged: {
                    if(state=="warning"){
                        state = "normal"
                    }
                }
            }

            DTextInput {
                id: repeat_input
                echoMode: detail_view.echoMode
                
                property bool warningState: false

                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.verticalCenter: parent.verticalCenter

                onTextChanged: {
                    warningState = false

                    if (text != "" && new_password_input.text.indexOf(text,0) != 0){
                        warningState = true
                        showWarning(dsTr("The two passwords do not match."))
                    }
                }

                onFocusChanged: {
                    if (focus){
                        warningState = false
                    }
                    else if (new_password_input.text != "" && repeat_input.text == "") {
                        warningState = true
                        showWarning(dsTr("The two passwords do not match."))
                    }
                }
                
                function showWarning(msg) {
                    var input_coord = repeat_input.mapToItem(root, 0, 0)
                    
                    warning_arrow_rect.x = input_coord.x - warning_arrow_rect.shadowWidth
                    warning_arrow_rect.y = input_coord.y + repeat_input.height - 4
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
                    }
                }
            }
        }

        DSeparatorHorizontal {}

        anchors.right: parent.right
    }
    
    WarningArrowRect {
        id: warning_arrow_rect
        visible: new_password_input.warningState || repeat_input.warningState
        width: new_password_input.width + shadowWidth * 2
    }    
}
