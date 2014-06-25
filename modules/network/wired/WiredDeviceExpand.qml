import QtQuick 2.0
import Deepin.Widgets 1.0

DBaseExpand {
    id: wiredDeviceExpand
    width: parent.width
    visible: wiredDevicesNumber > 0

    property var wiredDevices: {
        var tmp = nmDevices[nmDeviceTypeEthernet]
        if(typeof(tmp) == "undefined"){
            return new Array()
        }
        else{
            return tmp
        }
    }
    property int wiredDevicesNumber: wiredDevices.length

    property var dslConnections: {
        var tmp = nmConnections[nmConnectionTypePppoe]
        if(typeof(tmp) == "undefined"){
            return new Array()
        }
        else{
            return tmp
        }
    }
    property int dslConnectionNumber: dslConnections.length

    expanded: dbusNetwork.wiredEnabled

    header.sourceComponent: DBaseLine{
        id: wiredDeviceHeader
        leftLoader.sourceComponent: DssH2 {
            anchors.verticalCenter: parent.verticalCenter
            text: dsTr("Wired Network")
        }

        rightLoader.sourceComponent: DSwitchButton{
            checked: wiredDeviceExpand.expanded
            onClicked: {
                dbusNetwork.wiredEnabled = checked
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
                for(var i=0; i<wiredDevicesNumber;i++){
                    model.append({
                        "conn_index": i,
                        "dsl_index": -1, 
                    })
                    for(var j=0;j<dslConnectionNumber;j++){
                        model.append({
                            "conn_index": i,
                            "dsl_index": j, 
                        })
                    }
                }
            }
        }

    }
}
