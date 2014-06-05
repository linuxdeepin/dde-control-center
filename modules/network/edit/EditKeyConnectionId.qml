import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditKey {
    id: root
    Row {
        anchors.verticalCenter: parent.verticalCenter
        DTextInput {
            visible: connectionSession.type != nmConnectionTypeWired
            Binding on text {
                when: root.cacheValue != undefined
                value: root.cacheValue
            }
            onTextChanged: {
                root.cacheValue = text
                setKey()
            }
        }
        DLabel {
            visible: connectionSession.type == nmConnectionTypeWired
            Binding on text {
                when: root.cacheValue != undefined
                value: root.cacheValue
            }
        }
    }
}

