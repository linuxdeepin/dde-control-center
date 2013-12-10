import QtQuick 2.1
import "../widgets"

Rectangle {
    id: root
    color: constants.bgColor
    width: 310
    height: 600

    property variant constants: DConstants {}

    DColumn {
        DssTitle {
            text: dsTr("Account")
        }
        
        DBaseHeader {
            title.sourceComponent: DssH2 {
                text: dsTr("User List")
            }
            button.sourceComponent: Row {
                DImageButton {
                    normal_image: "../account/images/delete_normal.png"
                    hover_image: "../account/images/delete_hover.png"
                    press_image: "../account/images/delete_press.png"
                }
                
                DImageButton {
                    normal_image: "../account/images/add_normal.png"
                    hover_image: "../account/images/add_hover.png"
                    press_image: "../account/images/add_press.png"
                }
            }
        }
        
        UserList {}
    }
}