import QtQuick 2.0
import Deepin.Widgets 1.0

// TODO remove
Item {
    id: wiredDevicesArea
    width: parent.width
    height: childrenRect.height

    ListView{
        id: wiredDevicesList
        width: parent.width
        height: childrenRect.height
        model: dbusNetwork.wiredDevices

        delegate: WiredDeviceExpand{}
    }
}
