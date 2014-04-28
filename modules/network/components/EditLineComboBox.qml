import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: DComboBox{
        id: comboBox
        anchors.left: parent.left
        anchors.leftMargin: -3
        width: valueWidth
        
        Binding on text {
            when: root.value != undefined
            value: getAvailableValuesTextByValue(root.value)
        }
        
        property var menuLabels
        Connections {
            target: root
            onVisibleChanged: {
                if (root.visible) {
                    menuLabels = getAvailableValuesText()
                }
            }
        }

        function menuSelect(i){
            root.value = getAvailableValues()[i].Value
            setKey()
        }

        // TODO select current item when popup root menu
        onClicked: {
            if(!rootMenu.visible){
                menuLabels = getAvailableValuesText() // update menu labels
                var pos = mapToItem(null, 0, 0)
                rootMenu.labels = comboBox.menuLabels
                rootMenu.requestMenuItem = comboBox
                rootMenu.posX = pos.x
                rootMenu.posY = pos.y + height
                rootMenu.innerWidth = width
            }
            rootMenu.visible = !rootMenu.visible
        }
    }
}
