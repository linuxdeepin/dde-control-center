import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: DEditComboBox {
        id: comboBox
        width: valueWidth
        anchors.left: parent.left
        selectIndex: -1
        parentWindow: rootWindow

        Connections {
            target: root
            onWidgetShown: {
                text = root.cacheValue
                labels = root.getAvailableValuesText()
                values = root.getAvailableValuesValue()
                selectIndex = root.getAvailableValuesIndex()
            }
            onCacheValueChanged: {
                text = root.cacheValue
                labels = root.getAvailableValuesText()
                values = root.getAvailableValuesValue()
                selectIndex = root.getAvailableValuesIndex()
            }
        }

        onTextChanged: {
            root.cacheValue = text
            setKey()
        }
    }
}
