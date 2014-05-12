import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: DComboBox{
        id: comboBox
        anchors.left: parent.left
        anchors.leftMargin: -3
        width: valueWidth
        property var menuLabels
        
        Connections {
            target: root
            onWidgetShown: {
                text = getAvailableValuesTextByValue()
            }
            onValueChanged: {
                text = getAvailableValuesTextByValue()
            }
        }

        function menuSelect(i){
            root.value = getAvailableValues()[i].Value
            setKey()
        }

        onClicked: {
            if(!rootMenu.visible){
                menuLabels = getAvailableValuesText() // update availabel values
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
