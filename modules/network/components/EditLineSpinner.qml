import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    property int min: 0
    property int max: 65535
    
    rightLoader.sourceComponent: DSpinner {
        id: spinner
        width: valueWidth / 1.5
        min: root.min
        max: root.max
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
