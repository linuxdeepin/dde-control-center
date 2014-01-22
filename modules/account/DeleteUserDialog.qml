import QtQuick 2.1
import Deepin.Widgets 1.0

Column {
    id: root
    property var constants: DConstants{}
    
    signal cancel
    signal confirm (bool deleteFiles)
    
    Rectangle {
        width: parent.width
        height: parent.height / 2
        color: constants.bgColor
        
        DRadioButton {
            id: radio_button
            
            buttonModel: [
                {"buttonId": "keep_files", "buttonLabel": dsTr("Keep user files")},
                {"buttonId": "delete_files", "buttonLabel": dsTr("Delete user files")},
            ]
            
            anchors.centerIn: parent
        }
    }
    
    DSeparatorHorizontal{}

    Rectangle {
        width: parent.width
        height: parent.height / 2
        color: constants.bgColor
        
        DTextButton {
            id: cancel_button
            text: "Cancel"
            
            anchors.right: confirm_button.left
            anchors.rightMargin: 5
            anchors.verticalCenter: parent.verticalCenter
            
            onClicked: {
                root.cancel()
            }
        }
        
        DTextButton {
            id: confirm_button
            text: "Confirm"
            
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter            
            
            onClicked: {
                root.confirm(radio_button.currentIndex == 1)
            }
        }
    }
}