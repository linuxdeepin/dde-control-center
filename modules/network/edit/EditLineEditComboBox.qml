import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: DEditComboBox {
        id: comboBox
        width: valueWidth
        anchors.left: parent.left
        menu.labels: getAvailableValuesText()
        selectIndex: getAvailableValuesIndex()
        parentWindow: rootWindow

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
        
        Connections {
            target: root
            onVisibleChanged: {
                if (root.visible) {
                    comboBox.menu.labels = getAvailableValuesText()
                }
            }
        }

        onMenuSelect: {
            root.value = getAvailableValues()[index].Value
            setKey()
        }

    }
}
