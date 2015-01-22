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
            state: root.showError ? "warning" : "normal"
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
            elide: Text.ElideRight
            font.pixelSize: 12
            visible: connectionSession.type == nmConnectionTypeWired
            Binding on text {
                when: root.cacheValue != undefined
                value: root.cacheValue
            }
        }
    }
}
