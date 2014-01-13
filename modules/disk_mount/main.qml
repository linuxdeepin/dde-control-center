import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
    id: disk_mount
    anchors.fill: parent

    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Disk Mount")
        }

        DSeparatorHorizontal {}

        DBaseExpand {
            id: internal_expand

            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Internal Devices")
                onClicked: {
                    internal_expand.expanded = !internal_expand.expanded
                }
            }

            content.sourceComponent: DeviceList {
                width: disk_mount.width
            }
        }

        DSeparatorHorizontal {visible: !internal_expand.expanded}

        DBaseExpand {
            id: external_expand

            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Internal Devices")
                onClicked: {
                    external_expand.expanded = !external_expand.expanded
                }
            }

            content.sourceComponent: DeviceList {
                width: disk_mount.width
            }
        }

        DSeparatorHorizontal {visible: !external_expand.expanded}
    }
}
