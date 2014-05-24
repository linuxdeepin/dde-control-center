import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    rightLoader.sourceComponent: DSwitchButtonHeader{
        // TODO
        // anchors.left: parent.left
        // anchors.leftMargin: 100
        // leftMargin: 50
        Connections {
            target: root
            onWidgetShown: {
                active = root.value
            }
            onValueChanged: {
                active = root.value
            }
        }
        onActiveChanged: {
            root.value = active
            setKey()
        }
    }
}
