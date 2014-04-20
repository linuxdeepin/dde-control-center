import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    rightLoader.sourceComponent: DSwitchButtonHeader{
        onActiveChanged: {
            root.value = active
            setKey()
        }
        Component.onCompleted: {
            if (root.visible) {
                active = getKey()
            }
        }
    }
}
