import QtQuick 2.1
import "../widgets"

Rectangle {
    id: root
    color: constants.bgColor
    width: 310
    height: 900

    property variant constants: DConstants {}

    Column {
        DssTitle {
            text: dsTr("Account")
        }

        DSeparatorHorizontal{}

        DBaseHeader {
            title.sourceComponent: DssH2 {
                text: dsTr("User List")
            }
            button.sourceComponent: Row {
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
                }
            }
        }

        DSeparatorHorizontal{}

        UserList {
            id: user_list
        }
    }
}