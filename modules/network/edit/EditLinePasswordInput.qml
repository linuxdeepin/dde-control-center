import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    property var echoMode       // TODO remove
    
    rightLoader.sourceComponent: DTextInput{
        id: textInput
        width: valueWidth
        echoMode: TextInput.Password
        Binding on echoMode {
            target: root
            value: root.echoMode
        }
        Connections {
            target: root
            onWidgetShown: {
                text = root.value
            }
            onValueChanged: {
                text = root.value
            }
        }
        onTextChanged: {
            root.value = text
            setKey()
        }
    }
    
}

