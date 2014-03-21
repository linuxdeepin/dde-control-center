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
    header.sourceComponent: DSwitchButtonHeader {
        text: dsTr("Wireless Access Points") + dev[1]
        active: c2.expanded
        onActiveChanged: {
            nm.wirelessEnabled = active
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
