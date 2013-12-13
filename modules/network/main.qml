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

    DBaseLine {
        id:header
        height: 50
        leftLoader.sourceComponent: DLabel {
                text: dsTr("Network Settings")
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        stackView.pop(null)
                    }
                }
            }
        rightLoader.sourceComponent: Row {
            DTextButton {
                text: dsTr("Create Connections")
            }
            DTextButton {
                id: abc
                text: "i"
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        stackView.push(Qt.resolvedUrl("Info.qml"))
                    }
                }
            }
        }
    }

    StackView {
        id:stackView
        anchors.top: header.bottom
        initialItem: currenItem
    }
}
