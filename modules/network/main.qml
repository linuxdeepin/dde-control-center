import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import DBus.Com.Deepin.Daemon.Network 1.0
import Deepin.Widgets 1.0
import DGui 1.0

Column{
    id: networkModule
    anchors.fill: parent

    property variant nm: NetworkManager{}

    signal needSecretsEmit(string path, string encryptionName, string accessPointName)
    property bool inPasswordInputting: false

    property bool inAllConnectionPage: stackView.depth == 1
    property Item allConnectionPage: ListConnections {
        width: parent.width
        height: parent.height - header.height - 2
    }

    Component {
        id: connectionSession
        ConnectionSession {}
    }

    DBaseLine {
        id:header
        height: 48

        leftLoader.sourceComponent: DssH1 {
            text: dsTr("Network Settings")
            color: "white"
            font.bold: true
        }

        rightLoader.sourceComponent: Row {
            height: header.height
            spacing: 4

            DssAddButton{
                anchors.verticalCenter: parent.verticalCenter
            }

            DTextButton {
                id: abc
                text: "info"
                anchors.verticalCenter: parent.verticalCenter
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
