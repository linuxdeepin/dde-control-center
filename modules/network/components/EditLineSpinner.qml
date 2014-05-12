import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    // TODO
    property int min: rightLoader.item.min
    property int max: rightLoader.item.max
    
    rightLoader.sourceComponent: DSpinner {
        id: spinner
        width: valueWidth / 1.5
        Connections {
            target: root
            onWidgetShown: {
                text = String(root.value)
            }
            onValueChanged: {
                text = String(root.value)
            }
        }
        onTextChanged: {
            if (text != "") {
                root.value = parseInt(text)
                setKey()
            }
        }
    }
}
