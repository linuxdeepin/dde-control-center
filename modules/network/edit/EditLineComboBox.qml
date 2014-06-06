import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: DComboBox{
        id: comboBox
        anchors.left: parent.left
        width: valueWidth
        selectIndex: -1
        parentWindow: rootWindow
        
        Connections {
            target: root
            onWidgetShown: {
                text = root.getAvailableValuesTextByValue()
                labels = root.getAvailableValuesText()
                selectIndex = root.getAvailableValuesIndex()
            }
            onCacheValueChanged: {
                text = root.getAvailableValuesTextByValue()
                labels = root.getAvailableValuesText()
                selectIndex = root.getAvailableValuesIndex()
            }
        }

        onMenuSelect: {
            root.cacheValue = getAvailableValues()[index].Value
            setKey()
        }
    }
}
