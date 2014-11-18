import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    id:wiredArea
    width: parent.width

    property var wiredDevices: {
        var tmp = nmDevices[nmDeviceTypeEthernet]
        if(typeof(tmp) == "undefined"){
            return new Array()
        }
        else{
            return tmp
        }
    }
    property int wiredDevicesNumber: {
        if (wiredDevices) {
            return wiredDevices.length
        } else {
            return 0
        }
    }

    ListView{
        id: wiredDevicesList
        width: parent.width
        height: childrenRect.height
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        model: wiredDevicesNumber
        delegate: WiredDeviceExpand {
            conn_index: index
            wiredDevicesNumber: wiredArea.wiredDevicesNumber
            device: wiredDevices[index]
            DSeparatorHorizontal {
                height: 1
                anchors.top:parent.top
            }
        }
    }

    DSeparatorHorizontal {
        visible: wiredDevicesList.count > 0
    }
}
