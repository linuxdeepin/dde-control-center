import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0


DBaseExpand {
    id: c2
    width: parent.width

    property variant dev
    property variant accessPoints : nm.GetAccessPoints(dev[0])
    property bool freshPoints: false

    expanded: dev[1] != 20
    header.sourceComponent: DBaseLine{
        leftLoader.sourceComponent: Item {
            height: parent.height
            width: childrenRect.width

            WaitingImage{
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                on: dev[1] != 30 && dev[1] != 100
            }

            DssH1 {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                text: {
                    if(nm.wirelessDevices.length < 2){
                        return dsTr("Wireless Device")
                    }
                    else{
                        return dsTr("Wireless Device %1").arg(index + 1)
                    }
                }
            }
        }

        rightLoader.sourceComponent: DSwitchButton{
            checked: c2.expanded
            onClicked: {
                nm.wirelessEnabled = checked
            }
        }
    }

    content.sourceComponent: Item {
        width: parent.width
        height: childrenRect.height
        ListView {
            width: parent.width
            height: childrenRect.height
            model: c2.accessPoints
            delegate: WirelessItem {}
            Component.onCompleted: {
            }
        }

    }

    Timer {
        running: c2.expanded
        interval: 1000
        onTriggered : {
            if (freshPoints)
                c2.accessPoints = nm.GetAccessPoints(dev[0])
        }
    }
}
