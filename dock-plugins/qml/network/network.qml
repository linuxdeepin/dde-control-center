import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Network 1.0

DockQuickWindow {
    id: root
    title: "DSS"
    appid: "Applet100663307"
    icon: "images/icon.png"
    width: 300 
    height: childrenRect.height + 10
    color: Qt.rgba(0, 0, 0, 0.85)

    property var dbusNetwork: NetworkManager{}

    Item {
        anchors.centerIn: parent
        width: parent.width - 10
        height: childrenRect.height

        ListView {
        }
    }
}
