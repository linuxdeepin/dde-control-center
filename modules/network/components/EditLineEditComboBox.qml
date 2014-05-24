import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: DEditComboBox {
        id: comboBox
        width: valueWidth
        anchors.left: parent.left
        Connections {
            target: root
            onWidgetShown: {
                text = root.value
            }
            onValueChanged: {
                text = root.value
            }
        }
        onTextChanged: {
            root.value = text
            setKey()
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

        onClicked: {
            if(!rootMenu.visible){
                menuLabels = getAvailableValuesText() // update menu labels
                var pos = mapToItem(null, 0, 0)
                rootMenu.labels = comboBox.menuLabels
                rootMenu.requestMenuItem = comboBox
                rootMenu.currentIndex=getAvailableValuesIndex() // TODO
                rootMenu.posX = pos.x
                rootMenu.posY = pos.y + height
                rootMenu.innerWidth = width
            }
            rootMenu.visible = !rootMenu.visible
        }
    }
}
