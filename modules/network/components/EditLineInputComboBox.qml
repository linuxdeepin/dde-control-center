import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    // TODO custom widget editable combo box
    rightLoader.sourceComponent: DTextInput{
        width: valueWidth
        Binding on text {
            when: root.value != undefined
            value: root.value
        }
        // TODO
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
    // rightLoader.sourceComponent: DComboBox{
    //     id: comboBox
    //     anchors.left: parent.left
    //     anchors.leftMargin: -3
    //     width: valueWidth
    //     // TODO
    //     // Binding on text {
    //     //     when: root.value != undefined
    //     //     value: getAvailableValuesTextByValue()
    //     // }
    //     // text: root.value ? getAvailableValuesTextByValue() : ""
    //     text: {
    //         if (root.visible) {
    //             return getAvailableValuesTextByValue()
    //         } else {
    //             return ""
    //         }
    //     }
        
    //     property var menuLabels
    //     Connections {
    //         target: root
    //         onVisibleChanged: {
    //             if (root.visible) {
    //                 menuLabels = getAvailableValuesText()
    //             }
    //         }
    //     }

    //     function menuSelect(i){
    //         root.value = getAvailableValues()[i].Value
    //         setKey()
    //     }

    //     onClicked: {
    //         if(!rootMenu.visible){
    //             menuLabels = getAvailableValuesText() // update menu labels
    //             var pos = mapToItem(null, 0, 0)
    //             rootMenu.labels = comboBox.menuLabels
    //             rootMenu.requestMenuItem = comboBox
    //             rootMenu.currentIndex=getAvailableValuesIndex() // TODO
    //             rootMenu.posX = pos.x
    //             rootMenu.posY = pos.y + height
    //             rootMenu.innerWidth = width
    //         }
    //         rootMenu.visible = !rootMenu.visible
    //     }
    // }
}
