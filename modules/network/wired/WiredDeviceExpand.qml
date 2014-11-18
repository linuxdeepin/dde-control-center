import QtQuick 2.0
import Deepin.Widgets 1.0

DBaseExpand {
    id: wiredDeviceExpand
    width: parent.width
    height: visible ? childrenRect.height : 0
    visible: !(device.State == nmDeviceStateUnavailable && device.Vendor.indexOf("Apple") >= 0)
    expanded: dbusNetwork.IsDeviceEnabled(device.Path) && visible

    property int conn_index:0
    property int wiredDevicesNumber:0
    property int dslConnectionNumber: {
        if (dslConnections)
            return dslConnections.length
        else
            return 0
    }
    property var dslConnections:nmConnections[nmConnectionTypePppoe]
    property var device

    Connections{
        target: dbusNetwork
        onDeviceEnabled:{
            if(arg0 == device.Path){
                // print("onDeviceEnabled:", arg0, arg1) // TODO test
                wiredDeviceExpand.expanded = arg1
            }
        }
    }

    header.sourceComponent: DBaseLine{
        id: wiredDeviceHeader
        leftLoader.sourceComponent: DssH2 {
            elide: Text.ElideRight
            width: wiredDeviceExpand.width * 2 / 3
            anchors.verticalCenter: parent.verticalCenter
            text: wiredDevicesNumber > 1 && device.UsbDevice ? device.Vendor : dsTr("Wired Network")
        }

        rightLoader.sourceComponent: DSwitchButton{
            checked: wiredDeviceExpand.expanded
            Connections {
                // TODO still need connections block here, but why?
                target: wiredDeviceExpand
                onExpandedChanged: {
                    checked = wiredDeviceExpand.expanded
                }
            }
            onClicked: {
                dbusNetwork.EnableDevice(device.Path,checked)
            }
        }
    }

    content.sourceComponent: Item {
        width: parent.width
        height: childrenRect.height

        ListView {
            id: wiredListView
            width: parent.width
            height: childrenRect.height
            boundsBehavior: Flickable.StopAtBounds
            delegate: NewItem {}
            model: ListModel {}

            Connections {
                target: wiredDeviceExpand
                onWiredDevicesNumberChanged: {
                    wiredListView.updateModel()
                }
                onDslConnectionNumberChanged: {
                    wiredListView.updateModel()
                }
            }

            Component.onCompleted: {
                updateModel()
            }

            function updateModel(){
                model.clear()
                model.append({
                    "conn_index":wiredDeviceExpand.conn_index,
                    "dsl_index": -1
                })
                for(var i=0;i<dslConnectionNumber;i++){
                    if (dslConnections[i].HwAddress == "" || dslConnections[i].HwAddress == device.HwAddress){
                        model.append({
                            "conn_index":wiredDeviceExpand.conn_index,
                            "dsl_index": i
                        })
                    }
                }
            }
        }
    }

}
