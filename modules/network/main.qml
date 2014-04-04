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

    property var dbusNetwork: NetworkManager{}
    property var activeConnections: dbusNetwork.activeConnections

    signal needSecretsEmit(string path, string encryptionName, string accessPointName)

    property bool inPasswordInputting: false

    //property bool inAllConnectionPage: false
    property bool inAllConnectionPage: stackView.depth == 1
    property var allConnectionPage: ListConnections {}

    function isInList(s, sList){
        for(var i in sList){
            if (s == sList[i]){
                return i
            }
        }
        return -1
    }

    Component {
        id: connectionSession
        ConnectionSession {}
    }

    DssTitle {
        id:header
        height: 48
        text: dsTr("Network Settings")
        rightLoader.sourceComponent: Row {
            height: header.height
            spacing: 4

            DssAddButton{
                anchors.verticalCenter: parent.verticalCenter
            }

            DTextButton {
                text: "Info"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    stackView.push(Qt.resolvedUrl("Info.qml"))
                }
            }
        }
    }

    DSeparatorHorizontal{}

    StackView {
        id:stackView
        width: parent.width
        height: parent.height - header.height - 2
        initialItem: allConnectionPage
    }
}
