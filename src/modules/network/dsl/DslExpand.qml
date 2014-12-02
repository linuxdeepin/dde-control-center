import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    width: parent.width
    visible: dslConnectionNumber > 0

    property var dslConnections: nmConnections[nmConnectionTypePppoe]
    property int dslConnectionNumber: dslConnections ? dslConnections.length : 0

    DBaseExpand {
        id: dslExpand

        expanded: true
        header.sourceComponent: DBaseLine {
            id: wiredDeviceHeader
            leftLoader.sourceComponent: DssH2 {
                anchors.verticalCenter: parent.verticalCenter
                text: dsTr("PPPoE Connections")
            }

            rightLoader.sourceComponent: DSwitchButton{
                checked: dslExpand.expanded
                onClicked: {
                    dslExpand.expanded = !dslExpand.expanded
                    //dbusNetwork.dslEnabled = checked
                }
            }
        }
        content.sourceComponent: Item {
            width: parent.width
            height: childrenRect.height

            ListView {
                width: parent.width
                height: childrenRect.height
                boundsBehavior: Flickable.StopAtBounds
                model: dslConnectionNumber
                delegate: DslItem {}
            }
        }
    }

    DSeparatorHorizontal {}
}
