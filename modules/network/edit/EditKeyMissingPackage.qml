import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditKey {
    // TODO clickable pacakge text, and jump to software center
    id: root
    DssH2 {
        anchors.verticalCenter: parent.verticalCenter
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

