import QtQuick 2.1
import DBus.Com.Deepin.Daemon.Grub2 1.0
import DBus.Com.Deepin.Daemon.Display 1.0
import Deepin.Widgets 1.0

Item {
    id: root
    width: 310
    height: background.sourceSize.height / background.sourceSize.width * width

    Display { id: dbus_display }
    Grub2 { id: dbus_grub2 }
    Theme { id: dbus_grub2_theme }

    Image {
        id: background
        asynchronous: true
        source: "file://" + dbus_grub2_theme.background
        anchors.fill: parent
        cache: false

        property real scaleRatio: width / sourceSize.width

        function reload(){
            source = ""
            source = "file://" + dbus_grub2_theme.background
        }

        Connections {
            target: dbus_grub2_theme
            onBackgroundChanged: {
                print("onBackgroundChanged", dbus_grub2_theme.background)
                background.reload()
            }
        }

        DropArea {
            anchors.fill: parent

            onDropped: {
                if (drop.hasUrls) {
                    var ok = dbus_grub2_theme.SetBackgroundSourceFile(drop.urls[0])
                    if (!ok) {
                        tooltip.showText(tooltip.textImageNotSupport)
                        finishUpdatingTimer.restart()
                    }
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

            highlight: Rectangle { color: Qt.rgba(1, 1, 1, 0.1); radius: 2 }

            model: ListModel {}
            delegate: Item {
                width: 220
                height: entry_name.implicitHeight + 4
                clip: true

                Text {
                    id: entry_name
                    text: itemTitle
                    font.pixelSize: 7
                    elide: Text.ElideRight
                    color: itemTitle == dbus_grub2.defaultEntry ? dbus_grub2_theme.selectedItemColor : dbus_grub2_theme.itemColor

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
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
    }

    Rectangle {
        id: tooltip
        width: parent.width + 2 * radius
        height: txt.implicitHeight + 6
        radius: 5
        color: Qt.rgba(0.1, 0.1, 0.1, 0.5)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom

        property string textNormal: dsTr("Drag and drop an image to change background.")
        property string textImageNotSupport: dsTr("Image format not supported")
        property string textUpdating: dsTr("Updating...")
        property string textUpdated: dsTr("Successfully updated, reboot to view.")

        property color colorNotice: "#F48914"
        property color colorNormal: "white"
        property bool updating: dbus_grub2.updating || dbus_grub2_theme.updating
        onUpdatingChanged: {
            if(updating){
                showText(textUpdating)
            }
            else{
                showText(textUpdated)
                finishUpdatingTimer.restart()
            }
        }
        Component.onCompleted: {
            if (txt.text == "") {
                showText(textNormal)
            }
        }

        function showText(text) {
            if (text == textNormal) {
                txt.color = colorNormal
            } else {
                txt.color = colorNotice
            }
            txt.text = text
        }

        DssH2{
            id: txt
            anchors.centerIn: parent
        }
    }
    Timer {
        id: finishUpdatingTimer
        interval: 3000
        onTriggered: {
            tooltip.showText(tooltip.textNormal)
        }
    }
}
