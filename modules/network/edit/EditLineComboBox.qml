import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: DComboBox{
        id: comboBox
        anchors.left: parent.left
        width: valueWidth
        menu.labels: getAvailableValuesText()
        selectIndex: getAvailableValuesIndex()
        parentWindow: rootWindow
        
        Connections {
            target: root
            onWidgetShown: {
                text = getAvailableValuesTextByValue()
            }
            onCacheValueChanged: {
                text = getAvailableValuesTextByValue()
            }
        }

        onMenuSelect: {
            root.cacheValue = getAvailableValues()[index].Value
            setKey()
        }
    }
}
