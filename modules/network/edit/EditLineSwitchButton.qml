import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: DSwitchButtonHeader{
        anchors.right: parent.right
        rightMargin: 0
        Connections {
            target: root
            onWidgetShown: {
                active = root.cacheValue
            }
            onCacheValueChanged: {
                active = root.cacheValue
            }
        }
        onActiveChanged: {
            setKey(active)
        }
    }
}
