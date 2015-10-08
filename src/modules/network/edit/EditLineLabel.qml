import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: DLabel {
        width: valueWidth
        elide: Text.ElideRight
        font.pixelSize: 12
        Binding on text {
            when: root.cacheValue != undefined
            value: root.cacheValue
        }
    }
}

