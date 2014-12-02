import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root

    property int min: 0
    property int max: 65535

    rightLoader.sourceComponent: DSpinner {
        id: spinner
        activeFocusOnTab: true
        width: valueWidth / 1.5
        state: root.showError ? "warning" : "normal"
        min: root.min
        max: root.max

        property bool loaded: false
        Component.onCompleted: loaded = true

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
            if (loaded && text != "") {
                setKey(parseInt(text))
            }
        }
    }
}
