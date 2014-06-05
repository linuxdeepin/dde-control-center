import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditKey {
    id: root
    DComboBox {
        id: comboBox
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        property var menuLabels
        
        Connections {
            target: root
            onWidgetShown: {
                text = getAvailableValuesTextByValue()
            }
            onCacheValueChanged: {
                text = getAvailableValuesTextByValue()
            }
        }

        function menuSelect(i){
            root.cacheValue = getAvailableValues()[i].Value
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
