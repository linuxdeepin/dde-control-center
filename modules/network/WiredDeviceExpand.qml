import QtQuick 2.0
import Deepin.Widgets 1.0

DBaseExpand {
    id: wiredDeviceExpand
    width: parent.width

    expanded: dbusNetwork.wiredEnabled

    header.sourceComponent: DBaseLine{
        id: wiredDeviceHeader
        leftLoader.sourceComponent: DssH2 {
            anchors.verticalCenter: parent.verticalCenter
            text: dsTr("Wired Connetions")
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
            width: parent.width
            height: childrenRect.height
            model: dbusNetwork.wiredDevices
            delegate: WiredItem {}
        }

    }
}
