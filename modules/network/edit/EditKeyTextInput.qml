import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditKey {
    id: root
    DTextInput {
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
        onTextChanged: {
            root.cacheValue = text
            setKey()
        }
    }
}

