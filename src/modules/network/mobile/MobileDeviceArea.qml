import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    id:mobileArea
    width: parent.width

    property var mobileDevices: {
        var tmp = nmDevices[nmDeviceTypeModem]
        if(typeof(tmp) == "undefined"){
            return new Array()
        }
        else{
            return tmp
        }
    }
    property int mobileDevicenumber: {
        if (mobileDevices){
            return mobileDevices.length
        }
        else{
            return 0
        }
    }

    ListView{
        id: mobileDeviceList
        width: parent.width
        height: childrenRect.height
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        model: mobileDevicenumber
        delegate: MobileDeviceExpand {
            device: mobileDevices[index]
            DSeparatorHorizontal {
                height: 1
                anchors.top:parent.top
            }
        }
    }

    DSeparatorHorizontal {
        visible: mobileDeviceList.count > 0
    }
}
