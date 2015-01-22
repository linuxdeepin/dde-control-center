import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root

    rightLoader.sourceComponent: DTextInput{
        id: textInput
        activeFocusOnTab: true
        width: valueWidth
        state: root.showError ? "warning" : "normal"
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
