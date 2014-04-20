import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    rightLoader.sourceComponent: DSwitchButtonHeader{
        active: root.value
        onActiveChanged: {
            root.value = active
            setKey()
        }
    }
}


