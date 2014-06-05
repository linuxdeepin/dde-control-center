import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    // TODO clickable pacakge text, and jump to software center
    
    leftLoader.sourceComponent: DssH2{
        text: {
            if (root.text) {
                return root.text
            } else {
                return dsTr("Need Pacakge")
            }
        }
    }
    
    rightLoader.sourceComponent: DssH2 {
        Connections {
            target: root
            onWidgetShown: {
                text = root.cacheValue
            }
            onCacheValueChanged: {
                text = root.cacheValue
            }
        }
    }
}    

