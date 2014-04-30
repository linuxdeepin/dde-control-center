import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Locale 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Accounts 1.0
import DBus.Com.Deepin.SessionManager 1.0

Column {
    id: headerArea
    height: childrenRect.height
    width: parent.width

    property bool headerClicked: false

    property int sideWidth: 10

    property var dbusSessionManager: SessionManager {}
    property var accountId: Accounts {}
    property var currentUserObj: User { path: accountId.FindUserById(dbusSessionManager.currentUid) }

    function setAvatar(){
        avatarImage.imageSource = currentUserObj.iconFile
    }

    function setUserName(){
        userName.text = currentUserObj.userName
    }

    Connections {
        target: currentUserObj

        onIconFileChanged: {
            if(currentUserObj && currentUserObj.iconFile)
                setAvatar()
        }

        onUserNameChanged: {
            if (currentUserObj && currentUserObj.userName)
                setUserName()
        }
    }

    Rectangle {
        width: parent.width
        height: 150
        color: dconstants.contentBgColor

        DRoundImage {
            id: avatarImage
            roundRadius: 40
            anchors.top: parent.top
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                moduleIconList.iconClickAction("account")
                headerArea.headerClicked = true
            }
        }

        DssH1 {
            id: userName
            anchors.top: avatarImage.bottom
            anchors.topMargin: 8
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 18
            color: tuhaoColor
        }
    }

    DSeparatorHorizontal{}

    DBaseLine {}

    Timer{
        interval: 100
        running: true
        onTriggered: {
            setAvatar()
            setUserName()
        }
    }
}
