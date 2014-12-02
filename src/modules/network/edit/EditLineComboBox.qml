import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: DComboBoxSimple {
        id: comboBox
        anchors.left: parent.left
        width: valueWidth

        property var labels: new Array()
        property int selectIndex: -1
        
        onShowRequested: {
            if(!rootMenu.visible){
                rootMenu.labels = comboBox.labels
                rootMenu.requestMenuItem = comboBox
                rootMenu.currentIndex = selectIndex
                rootMenu.posX = x
                rootMenu.posY = y
                rootMenu.innerWidth = width
            }
            rootMenu.visible = !rootMenu.visible
        }

        function menuSelect(i){
            setKey(getAvailableValues()[i].Value)
        }
        
        Connections {
            target: root
            onWidgetShown: {
                comboBox.text = root.getAvailableValuesTextByValue()
                comboBox.labels = root.getAvailableValuesText()
                comboBox.selectIndex = root.getAvailableValuesIndex()
            }
            onCacheValueChanged: {
                comboBox.text = root.getAvailableValuesTextByValue()
                comboBox.labels = root.getAvailableValuesText()
                comboBox.selectIndex = root.getAvailableValuesIndex()
            }
        }
    }
}
