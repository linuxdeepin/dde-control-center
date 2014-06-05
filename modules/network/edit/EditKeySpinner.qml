import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditKey {
    id: root
    
    property int min: 0
    property int max: 65535
    
    DSpinner {
        id: spinner
        anchors.verticalCenter: parent.verticalCenter
        min: root.min
        max: root.max
        Connections {
            target: root
            onWidgetShown: {
                text = String(root.cacheValue)
            }
            onCacheValueChanged: {
                text = String(root.cacheValue)
            }
        }
        onTextChanged: {
            if (text != "") {
                root.cacheValue = parseInt(text)
                setKey()
            }
        }
    }
}
