import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../widgets"


DBaseExpand {
    property variant dev
    property variant accessPoints : nm.GetAccessPoints(dev[0])
    id: c2
    width: root.width
    expanded: dev[1] != 20
    header.sourceComponent: DSwitchButtonHeader {
        text: dsTr("Wireless Access Points" + dev[1])
        active: c2.expanded
        onActiveChanged: {
            nm.wirelessEnabled = active
        }
    }
    content.sourceComponent: Column {
        Repeater {
            model: c2.accessPoints
            delegate: DLabel {
                height: 30
                width: root.width
                verticalAlignment: Text.AlignVCenter
                text: accessPoints[index][0] + "  " + accessPoints[index][2] + "  " + accessPoints[index][1]
                DSeparatorHorizontal{}
            }
        }
        Timer {
            running: true
            interval: 1000
            onTriggered : {
                c2.accessPoints = nm.GetAccessPoints(dev[0])
            }
        }
    }
}
