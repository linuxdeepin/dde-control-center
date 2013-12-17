import QtQuick 2.1
import "../widgets"

Rectangle {
    id: root
    color: constants.bgColor
    width: 310
    height: 600

    property variant constants: DConstants {}

    Column {
        id: main_column
        state: "normal"

        DssTitle {
            id: module_title
            text: dsTr("Account")
        }

        DSeparatorHorizontal{}

        DBaseLine {
            id:title

            leftLoader.sourceComponent: DssH2 {
                text: dsTr("User List")
            }
            rightLoader.sourceComponent: Row {
                spacing: 10

                DImageCheckButton {
                    inactivatedNomralImage: "images/delete_normal.png"
                    inactivatedHoverImage: "images/delete_normal.png"
                    inactivatedPressImage: "images/delete_normal.png"

                    activatedNomralImage: "images/delete_press.png"
                    activatedHoverImage: "images/delete_press.png"
                    activatedPressImage: "images/delete_press.png"

                    onActivateChanged: {
                        if (activate) {
                            user_list.allAction()
                        } else {
                            user_list.allNormal()
                        }
                    }
                }

                DImageCheckButton {
                    inactivatedNomralImage: "images/add_normal.png"
                    inactivatedHoverImage: "images/add_normal.png"
                    inactivatedPressImage: "images/add_normal.png"

                    activatedNomralImage: "images/add_press.png"
                    activatedHoverImage: "images/add_press.png"
                    activatedPressImage: "images/add_press.png"

                    onActivateChanged: {
                        if (activate) {
                            main_column.state = "add_dialog"
                        } else {
                            main_column.state = "normal"
                        }
                    }
                }
            }
        }

        DSeparatorHorizontal{}

        AddUserDialog { id: add_user_dialog }

        UserList {
            id: user_list
        }

        move: Transition {
            NumberAnimation { properties: "y"; duration: 200 }
        }

        states: [
            State {
                name: "normal"

                /* PropertyChanges { */
                /*     target: user_list */
                /*     visible: true */
                /* } */
                PropertyChanges {
                    target: add_user_dialog
                    visible: false
                }
            },
            State {
                name: "add_dialog"

                /* PropertyChanges { */
                /*     target: user_list */
                /*     visible: false */
                /* } */
                PropertyChanges {
                    target: add_user_dialog
                    visible: true
                }
            }
        ]
    }
}