/*************************************************************
*File Name: SharePanel.qml
*Author: Match
*Email: Match.YangWanQing@gmail.com
*Created Time: 2015年04月10日 星期五 11时17分03秒
*Description:
*
*************************************************************/
import QtQuick 2.1
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Remoting.Server 1.0
import "./ShareContent"


Item {
    id: sharePanel
    width: parent.width
    height: 300

    state: "CreatingCode"

    //Server is uninitialized
    readonly property int serverStatusUninitialized : 0
    //Server is started
    readonly property int serverStatusStarted: 1
    //Server recieved new peer id
    readonly property int serverStatusPeerIdOk : 2
    //Server failed to get peer id
    //Caused by local network connection problem
    //This status is set 15s after server has been started and no valid peer id
    //received
    readonly property int  serverStatusPeerIdFailed : 3
    //Server is connected, screen is being shared
    readonly property int  serverStatusSharing : 4
    //Server is stopped
    readonly property int serverStatusStoped : 5
    //Remote peer has closed media connection
    readonly property int serverStatusDisconnected: 6

    // DBus root interface
    property var remotingServer: RemotingServer {}
    property var dconstants: DConstants {}

    Component.onCompleted: {
        remotingServer.Start()

        var tmpStatus= remotingServer.GetStatus()
        switch (tmpStatus){
        case serverStatusPeerIdOk:
            codeText.text = remotingServer.GetPeerId()
            sharePanel.state = "CreatedCode"
            break
        case serverStatusSharing:
            sharePanel.state = "Connected"
            break
        case serverStatusStoped:
            sharePanel.state = "CreatingCode"
            break
        case serverStatusPeerIdFailed:
            errorItem.setErrorMessage(dsT("Network error,create code failed!"))
            sharePanel.state = "error"
            break
        default:
            break
        }
    }

    Connections {
        target: remotingServer
        onStatusChanged: {
            switch (status){
            case serverStatusPeerIdOk:
                generatedCodeitem.setCodeText(remotingServer.GetPeerId())
                sharePanel.state = "CreatedCode"
                break
            case serverStatusSharing:
                sharePanel.state = "Connected"
                break
            case serverStatusStoped:
                sharePanel.state = "CreatingCode"
                reset()
                break
            case serverStatusPeerIdFailed:
                errorItem.setErrorMessage(dsT("Network error,create code failed!"))
                sharePanel.state = "error"
                break
            default:
                break
            }
        }
    }

    DssTitle {
        id:shareTitle
        anchors.top: parent.top
        text: dsTr("Sharing")
    }

    DSeparatorHorizontal {
        id:separator1
        anchors.top: shareTitle.bottom
    }

    GeneratingCodeItem {
        id: generatingCodeItem
        visible: sharePanel.state == "CreatingCode"
        enabled: visible
        width: parent.width
        height: 200
        anchors {top: separator1.bottom; horizontalCenter: parent.horizontalCenter}
    }

    GeneratedCodeItem {
        id: generatedCodeitem
        visible: sharePanel.state == "CreatedCode"
        enabled: visible
        width: parent.width
        height: 200
        anchors {top: separator1.bottom; horizontalCenter: parent.horizontalCenter}
    }

    ConnectedItem {
        id: connectedItem
        visible: sharePanel.state == "Connected"
        enabled: visible
        width: parent.width
        height: 200
        anchors {top: separator1.bottom; horizontalCenter: parent.horizontalCenter}
    }

    ErrorItem {
        id:errorItem
        visible: sharePanel.state == "error"
        enabled: visible
        width: parent.width
        height: 200
        anchors {top: separator1.bottom; horizontalCenter: parent.horizontalCenter}
        onRetryGenerateCode: sharePanel.state = "CreatingCode"
    }

    states:[
        State {
            name: "CreatingCode"
        },
        State {
            name: "CreatedCode"
        },
        State {
            name: "Connected"
        },
        State {
            name: "error"
        }

    ]
}
