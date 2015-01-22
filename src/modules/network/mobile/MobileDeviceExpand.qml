import QtQuick 2.0
import Deepin.Widgets 1.0

DBaseExpand {
    id: mobileDeviceExpand
    width: parent.width
    height: visible ? childrenRect.height : 0
    visible: device.State != nmDeviceStateUnavailable
    expanded: dbusNetwork.IsDeviceEnabled(device.Path) && visible

    property var device

    Connections{
        target: dbusNetwork
        onDeviceEnabled:{
            if(arg0 == device.Path){
                // print("onDeviceEnabled:", arg0, arg1) // TODO test
                mobileDeviceExpand.expanded = arg1
            }
        }
    }

    header.sourceComponent: DBaseLine{
        leftLoader.sourceComponent: DssH2 {
            elide: Text.ElideRight
            width: mobileDeviceExpand.width * 2 / 3
            anchors.verticalCenter: parent.verticalCenter
            text: device.Vendor
        }

        rightLoader.sourceComponent: DSwitchButton{
            checked: mobileDeviceExpand.expanded
            Connections {
                // TODO still need connections block here, but why?
                target: mobileDeviceExpand
                onExpandedChanged: {
                    checked = mobileDeviceExpand.expanded
                }
            }
            onClicked: {
                dbusNetwork.EnableDevice(device.Path,checked)
            }
        }
    }

    content.sourceComponent: MobileItem {
        mobileDevice: mobileDeviceExpand.device
    }

}
