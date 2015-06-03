import QtQuick 2.1
import QtQuick.Controls 1.0

import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Remoting.Manager 1.0
import "../shared/"

Item {
    id: remotingItem

    property var stackViewPages: {
        "mainPage": Qt.resolvedUrl("MainPanel.qml"),
        "accessPage": Qt.resolvedUrl("AccessPanel.qml"),
        "sharePage": Qt.resolvedUrl("SharePanel.qml")
    }

    property var remotingManager: RemotingManager {}

    // Neither client side nor server side is running
    readonly property int managerStatusUninitialized: 0

    // Client side is running
    readonly property int managerStatusClient: 1

    // Server side is running
    readonly property int managerStatusServer: 2

    readonly property int networkStatusUnknown: 0
    readonly property int networkStatusConnected: 1
    readonly property int networkStatusDisconnected: 2

    // Change page in stackview
    function changePage(page) {
        stackView.push({
            "item": stackViewPages[page],
            "properties": { "width": parent.width }
        })
        stackView.currentItemId = page
    }

    // Reset stackview
    function resetPage(){
        stackView.pop(null)
        stackView.currentItemId = "mainPage"
    }

    StackView {
        id:stackView
        width: parent.width
        height: parent.height

        property string currentItemId: ""

        Component.onCompleted: {
            // Loading main page
            changePage("mainPage")

            // Check remoting service status
            var managerStatus = remotingManager.GetStatus()

            switch (managerStatus) {
            case (managerStatusClient):
                changePage("accessPage")
                break

            case (managerStatusServer):
                changePage("sharePage")
                break

            default:
                break
            }
        }
    }
}
