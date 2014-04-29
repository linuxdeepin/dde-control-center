import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine{
    id: root
    
    property var nameFilters
    // property alias nameFilters: fileDialog.nameFilters
    
    rightLoader.sourceComponent: Row{
        width: valueWidth
        spacing: 6
        
        DTextInput{
            id: fileText
            width: valueWidth * 0.65 // TODO layout and width
            // TODO
            Binding on text {
                when: root.value != undefined
                value: root.value
            }
            // text: {
            //     if (root.value) {
            //         return root.value
            //     } else {
            //         return ""
            //     }
            // }
            onTextChanged: {
                root.value = text
                setKey()
            }
        }
        
        // TODO
        DTextButton{
            text: dsTr("...")
            onClicked: {
                // fileDialog.open()
            }
        }
        
        // FileDialog{
        //     id: fileDialog
        //     onAccepted: {
        //         fileText = fileUrl
        //     }
        // }
    }
}

