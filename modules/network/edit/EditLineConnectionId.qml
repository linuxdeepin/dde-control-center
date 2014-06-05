import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: Row {
    DTextInput {
        width: valueWidth
        visible: connectionSession.type != nmConnectionTypeWired
        // TODO
        // Connections {
        //     target: root
        //     onWidgetShown: {
        //         text = root.cacheValue
        //     }
        //     onCacheValueChanged: {
        //         text = root.cacheValue
        //     }
        // }
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
        width: valueWidth
        visible: connectionSession.type == nmConnectionTypeWired
        Binding on text {
            when: root.cacheValue != undefined
            value: root.cacheValue
        }
    }
    }
}

