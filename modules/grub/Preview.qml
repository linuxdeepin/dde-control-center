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
        }
        
        DropArea {
            anchors.fill: parent
            
            onDropped: {
                if (drop.hasUrls) {
                    dbus_grub2_theme.SetBackgroundSourceFile(drop.urls[0].substring(6))
                }
            }
        }

        Column {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 20
            anchors.topMargin: 20
            Repeater {
                model: ListModel {}
                delegate: Text {
                    text: itemTitle
                    font.pixelSize: 16 * background.scaleRatio
                    color: itemTitle == dbus_grub2.defaultEntry ? dbus_grub2_theme.selectedItemColor : dbus_grub2_theme.itemColor
                }

                Component.onCompleted: {
                    var entries = dbus_grub2.GetSimpleEntryTitles();
                    for (var i = 0; i < entries.length; i++) {
                        model.append({"itemTitle": entries[i]})
                    }
                }
            }
        }
    }
}