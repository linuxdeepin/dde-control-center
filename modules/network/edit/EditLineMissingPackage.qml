import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    // TODO clickable pacakge text, and jump to software center
    
    rightLoader.sourceComponent: DLabel {
        width: valueWidth
        Connections {
            target: root
            onWidgetShown: {
                text = root.value
            }
            onValueChanged: {
                text = root.value
            }
        }
    }
}

