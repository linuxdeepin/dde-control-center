import QtQuick 2.1
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.DiskMount 1.0

Item {
    id: disk_mount
    anchors.fill: parent

    DiskMount { id: dbus_mounts }

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

                function initList() {
                    model.clear()
                    var list = dbus_mounts.diskList;
                    for(var i = 0; i < list.length; i++) {
                        if (list[i][2] == "native") {
                            model.append({"deviceId": list[i][0],
                                          "deviceName": list[i][1],
                                          "deviceType": list[i][2],
                                          "deviceCanUnmount": list[i][3],
                                          "deviceCanEject": list[i][4],
                                          "deviceUsableCap": list[i][5],
                                          "deviceTotalCap": list[i][6]})
                        }
                    }
                }

                Component.onCompleted: {
                    initList()
                }
                
                Connections {
                    target: dbus_mounts
                    onDiskListChanged: {
                        initList()
                    }
                }
            }
        }

        DSeparatorHorizontal {visible: !internal_expand.expanded}

        DBaseExpand {
            id: external_expand

            header.sourceComponent: DDownArrowHeader {
                text: dsTr("External Devices")
                onClicked: {
                    external_expand.expanded = !external_expand.expanded
                }
            }

            content.sourceComponent: DeviceList {
                width: disk_mount.width
                
                function initList() {
                    model.clear()
                    var list = dbus_mounts.diskList;
                    for(var i = 0; i < list.length; i++) {
                        if (list[i][2] != "native") {
                            model.append({"deviceId": list[i][0],
                                          "deviceName": list[i][1],
                                          "deviceType": list[i][2],
                                          "deviceCanUnmount": list[i][3],
                                          "deviceCanEject": list[i][4],
                                          "deviceUsableCap": list[i][5],
                                          "deviceTotalCap": list[i][6]})
                        }
                    }
                }

                Component.onCompleted: {
                    initList()
                }
                
                Connections {
                    target: dbus_mounts
                    onDiskListChanged: {
                        initList()
                    }
                }
            }
        }

        DSeparatorHorizontal {visible: !external_expand.expanded}
    }
}
