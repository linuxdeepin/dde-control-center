import QtQuick 2.0
import Deepin.Widgets 1.0

DBaseExpand {
    id: wiredDeviceExpand
    width: parent.width

    expanded: nm.wiredEnabled

    header.sourceComponent: DBaseLine{
        id: wiredDeviceHeader
        leftLoader.sourceComponent: Row {
            height: parent.height
            spacing: 6

            ConnectStatusImage{
                currentStatusCode: nm.wiredDevices[index][1]
                anchors.verticalCenter: parent.verticalCenter
            }

            DssH1 {
                text: {
                    if(nm.wiredDevices.length < 2){
                        return "Wired Device"
                    }
                    else{
                        "Wired Device %1".arg(index + 1)
                    }
                }
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        rightLoader.sourceComponent: DSwitchButton{
            checked: wiredDeviceExpand.expanded
            onClicked: {
                nm.wiredEnabled = checked
            }
        }
    }

    content.sourceComponent: Item {
        width: parent.width
        height: childrenRect.height

        ListView {
            width: parent.width
            height: childrenRect.height
            model: 1
            delegate: WiredItem {}
            Component.onCompleted: {
            }
        }

    }
}
