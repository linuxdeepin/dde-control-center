import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    property var echoMode       // TODO remove
    
    rightLoader.sourceComponent: DTextInput{
        id: textInput
        activeFocusOnTab: true
        width: valueWidth
        state: root.showError ? "warning" : "normal"
        echoMode: TextInput.Password
        Binding on echoMode {
            target: root
            value: root.echoMode
        }
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
            setKey(text)
        }
    }
    
}

