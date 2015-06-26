/*************************************************************
*File Name: AccessPanel.qml
*Author: Match
*Email: Match.YangWanQing@gmail.com
*Created Time: 2015年04月10日 星期五 11时17分14秒
*Description:
*
*************************************************************/
import QtQuick 2.1
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Remoting.Client 1.0

import "./AccessContent"

Item {
    id: accessPanel
    width: parent.width
    height: 300

    state: "NeeedAccessCode"

    //DBus status
    //Client is uninitialized
    readonly property int clientStatusUninitialized: 0

    //Client window showed up
    readonly property int clientStatusStarted: 1

    //Client window is closed
    readonly property int clientStatusStopted: 2

    //Web page in browser is loaded and ready
    //Call Connect() method in dbus only if receviing this ready signal.
    //Or else oxide will be failed to setup message channel
    //and remote peer id will never be sent to browser side
    readonly property int clientStatusPageReady: 3

    //Connecting to remote peer (server side)
    readonly property int clientStatusConnecting: 4

    //Connectted to remote peer successfully
    readonly property int clientStatusConnectOk: 5

    //Failed to connect to remote peer
    readonly property int clientStatusConnectFailed: 6

    //peerId unavailable
    readonly property int clientStatusUnavailable: 7

    //Remote peer has closed desktop sharing
    readonly property int clientStatusDisconnected: 8

    // DBus root interface
    property var remotingClient: RemotingClient {}

    Component.onCompleted: {
        // If no network connection is available, display error page and exit
        if (remotingManager.CheckNetworkConnectivity() ===
                networkStatusDisconnected) {
            errorItem.setErrorMessage(dsTr("There is no network connection currently, please try again after you connect to the Internet"))
            accessPanel.state = "error"
            return
        }

        var clientStatus = remotingClient.GetStatus()
        switch (clientStatus){
        case clientStatusConnecting:
            accessPanel.state = "Connecting"
            break

        case clientStatusConnectOk:
            accessPanel.state = "Connected"
            break

        case clientStatusStopted:
            accessPanel.state = "NeeedAccessCode"
            break

        case clientStatusConnectFailed:
            needCodeItem.showError(dsTr("Failed to establish the connection, you can retry to connect"))
            accessPanel.state = "NeeedAccessCode"
            break

        default:
            break
        }
    }

    Connections {
        target: remotingClient
        onStatusChanged: {
            if (remotingManager.CheckNetworkConnectivity() ==
                    networkStatusDisconnected) {
                errorItem.setErrorMessage(dsTr("There is no network connection currently, please try again after you connect to the Internet"))
                accessPanel.state = "error"
                return
            }

            switch (status){
            case clientStatusConnecting:
                accessPanel.state = "Connecting"
                break

            case clientStatusConnectOk:
                accessPanel.state = "Connected"
                break

            case clientStatusStopted:
                if (accessPanele.state !== "NeedAccessCode") {
                    accessPanel.state = "NeeedAccessCode"
                }
                break

            case clientStatusPageReady:
                remotingClient.Connect(needCodeItem.code.trim())
                break

            case clientStatusUnavailable:
                needCodeItem.showError(dsTr("The verification code is invalid! Please retry"))
                accessPanel.state = "NeeedAccessCode"
                break

            case clientStatusConnectFailed:
                needCodeItem.showError(dsTr("Failed to establish the connection, you can retry to connect"))
                accessPanel.state = "NeeedAccessCode"
                break

            default:
                break
            }
        }
    }

    DssTitle {
        id:accessTitle
        anchors.top: parent.top
        text: dsTr("Accessing")
    }

    DSeparatorHorizontal {
        id:separator1
        anchors.top: accessTitle.bottom
    }

    NeedCodeItem {
        id: needCodeItem
        visible: accessPanel.state == "NeeedAccessCode"
        enabled: visible
        width: parent.width
        height: 200
        anchors {
            top: separator1.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }

    ConnectingItem {
        id: connectingItem
        visible: accessPanel.state == "Connecting"
        enabled: visible
        width: parent.width
        height: 200
        anchors {
            top: separator1.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }

    ConnectedItem {
        id: connectedItem
        visible: accessPanel.state == "Connected"
        enabled: visible
        width: parent.width
        height: 200
        anchors {
            top: separator1.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }

    ErrorItem {
        id: errorItem
        visible: accessPanel.state == "error"
        enabled: visible
        width: parent.width
        height: 200
        anchors {
            top: separator1.bottom
            horizontalCenter: parent.horizontalCenter
        }
        onRetryConnect: accessPanel.state = "NeeedAccessCode"
    }

    states:[
        State {
            name: "NeeedAccessCode"
        },
        State {
            name: "Connecting"
        },
        State {
            name: "Connected"
        },
        State {
            name: "error"
        }
    ]
}

