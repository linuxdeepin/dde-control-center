import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    width: parent.width
    WiredDeviceExpand {
        id: wiredExpand
    }

    DSeparatorHorizontal {
        visible: wiredExpand.visible
    }
}
