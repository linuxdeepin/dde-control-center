import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import DBus.Com.Deepin.Daemon.Network 1.0
import "../widgets"

Item {
    id:root
    property Item currenItem: ListConnections {}
    //property Item currenItem: Info {width: root.width}
    property variant nm: NetworkManager{}

    Header { id:header }
    StackView {
        id:stackView
        anchors.top: header.bottom
        initialItem: currenItem
    }
}
