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
                DImageButton {
                    normal_image: "images/delete_normal.png"
                    hover_image: "images/delete_hover.png"
                    press_image: "images/delete_press.png"
                }
                
                DImageButton {
                    normal_image: "images/add_normal.png"
                    hover_image: "images/add_hover.png"
                    press_image: "images/add_press.png"
                }
            }
        }
        
        DSeparatorHorizontal{}
        
        UserList {
            onHeightChanged: {
                print(height)
            }
        }
    }
}