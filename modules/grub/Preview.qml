import QtQuick 2.1
import DBus.Com.Deepin.Daemon.Grub2 1.0

Item {
    id: root
    width: 300
    height: 300

    Grub2 { id: dbus_grub2 }
    Theme { id: dbus_grub2_theme }

    Image {
        id: background
        smooth: true
        width: parent.width
        height: width * sourceSize.height / sourceSize.width
        source: dbus_grub2_theme.background

        property real scaleRatio: width / sourceSize.width

        Component.onCompleted: {
            root.width = width
            root.height = height

            print(sourceSize.width)
            print(sourceSize.height)
        }

        Connections {
            target: dbus_grub2_theme
            onBackgroundUpdated: {
                background.source = ""
                background.source = dbus_grub2_theme.background
            }
        }

        DropArea {
            anchors.fill: parent

            onDropped: {
                if (drop.hasUrls) {
                    dbus_grub2_theme.SetBackgroundSourceFile(drop.urls[0].substring(6))
                }
            }
        }

        ListView {
            id: entries_list
            width: 100
            height: root.height / 2

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 50
            anchors.topMargin: 50

            highlight: Rectangle { color: Qt.rgba(1, 1, 1, 0.3); radius: 2 }

            model: ListModel {}
            delegate: Item {
                width: 160
                height: entry_name.implicitHeight + 4

                Text {
                    id: entry_name
                    text: itemTitle
                    font.pixelSize: 7
                    color: itemTitle == dbus_grub2.defaultEntry ? dbus_grub2_theme.selectedItemColor : dbus_grub2_theme.itemColor

                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            function getCurrentIndex () {
                var entries = dbus_grub2.GetSimpleEntryTitles()
                for (var i = 0; i < entries.length; i++) {
                    if (entries[i] == dbus_grub2.defaultEntry) {
                        return i
                    }
                }
                return 0
            }

            Connections {
                target: dbus_grub2
                onDefaultEntryChanged: {
                    entries_list.currentIndex = entries_list.getCurrentIndex()
                }
            }

            Component.onCompleted: {
                var entries = dbus_grub2.GetSimpleEntryTitles();
                for (var i = 0; i < entries.length; i++) {
                    model.append({"itemTitle": entries[i]})
                    if (entries[i] == dbus_grub2.defaultEntry) {
                        currentIndex = i
                    }
                }
            }
        }

        Text {
            color: "#A34545"
            font.pixelSize: 7
            text: "Booting in " + dbus_grub2.timeout + " seconds"

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
        }
    }

    Rectangle {
        id: tooltip
        width: txt.implicitWidth + 16
        height: txt.implicitHeight + 16
        opacity: 0.0
        radius: 5
        color: Qt.rgba(0.1, 0.1, 0.1, 0.5)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 15

        Behavior on opacity {
            SmoothedAnimation { duration: 200 }
        }

        Text {
            id: txt
            color: "white"
            text: dsTr("Drop a picture here to change the background")

            anchors.centerIn: parent
        }
    }

    MouseArea {
        hoverEnabled: true
        anchors.fill: parent

        onEntered: {
            tooltip.opacity = 1.0
        }

        onExited: {
            tooltip.opacity = 0.0
        }
    }
}