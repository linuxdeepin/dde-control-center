import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditKey {
    id: root
    DSwitchButtonHeader {
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        rightMargin: 0 // TODO
        color: dconstants.contentBgColor
        
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
            root.cacheValue = active
            setKey()
        }
    }
}
