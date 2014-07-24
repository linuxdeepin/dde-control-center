/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

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
    property string userObjectPath: accountId.FindUserById(dbusSessionManager.currentUid)
    property var currentUserObj: User { path: userObjectPath }

    function setAvatar(){
        avatarImage.imageSource = "file://" + currentUserObj.iconFile
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
            if(userObjectPath){
                setAvatar()
                setUserName()
            }
            else{
                avatarImage.imageSource = "/var/lib/AccountsService/icons/guest_96.png"
                userName.text = dsTr("Guest")
            }
        }
    }
}
