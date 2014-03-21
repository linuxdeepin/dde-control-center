import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import DBus.Com.Deepin.Daemon.Network 1.0
import Deepin.Widgets 1.0

Column{
    id: networkModule
    width: parent.width
    height: parent.height

    property variant nm: NetworkManager{}

    signal needSecretsEmit(string path, string encryptionName, string accessPointName)
    property bool inPasswordInputting: false

    property bool inAllConnectionPage: stackView.depth == 1
    property Item allConnectionPage: ListConnections {
        width: parent.width
        height: parent.height - header.height - 2
    }

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

    DSeparatorHorizontal{}

    StackView {
        id:stackView
        initialItem: allConnectionPage
    }
}
