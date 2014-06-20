import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: DEditComboBoxSimple {
        id: comboBox
        activeFocusOnTab: true
        width: valueWidth
        state: root.showError ? "warning" : "normal"
        anchors.left: parent.left

        property var labels: new Array()
        property var values: new Array()
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
            if(i != -1)
                comboBox.text = values[i]
        }

        Connections {
            target: root
            onWidgetShown: {
                comboBox.text = root.cacheValue
                comboBox.labels = root.getAvailableValuesText()
                comboBox.values = root.getAvailableValuesValue()
                comboBox.selectIndex = root.getAvailableValuesIndex()
            }
            onCacheValueChanged: {
                comboBox.text = root.cacheValue
                comboBox.labels = root.getAvailableValuesText()
                comboBox.values = root.getAvailableValuesValue()
                comboBox.selectIndex = root.getAvailableValuesIndex()
            }
        }

        onTextChanged: {
            setKey(text)
        }
    }
}
