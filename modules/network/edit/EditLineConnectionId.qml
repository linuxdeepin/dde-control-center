import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    rightLoader.sourceComponent: Row {
        DTextInput {
            id: textInput
            activeFocusOnTab: true
            width: valueWidth
            visible: connectionSession.type != nmConnectionTypeWired
            Binding on text {
                when: root.cacheValue != undefined
                value: root.cacheValue
            }
            onTextChanged: {
                setKey(text)
            }
        }
        DLabel {
            width: valueWidth
            visible: connectionSession.type == nmConnectionTypeWired
            Binding on text {
                when: root.cacheValue != undefined
                value: root.cacheValue
            }
        }
    }
}

