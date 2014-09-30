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
import DBus.Com.Deepin.Daemon.Display 1.0
import DBus.Com.Deepin.Daemon.Network 1.0
import Deepin.Locale 1.0

Window {
    id: messageBox
    color: "transparent"
    flags: Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint

    width: 380
    height: 150
    visible: false
    x: screenSize.x + (screenSize.width - width)/2
    y: screenSize.y + (screenSize.height - height)/2

    property var displayId: Display {}
    property var screenSize: QtObject {
        property int x: displayId.primaryRect[0]
        property int y: displayId.primaryRect[1]
        property int width: displayId.primaryRect[2]
        property int height: displayId.primaryRect[3]
    }

    property string accessPointName: "linuxdepin-1"
    property string accessPointEncryption: ""
    property bool autoconnect: false
    property var accessPointObj: undefined
    property var connectionPath: undefined

    property var dbusNetwork: NetworkManager{
        onNeedSecrets:{
            print("NeedSectets Emit in dss Frame:", arg0, arg1, arg2,arg3)
            if(!dbusControlCenter.isNetworkCanShowPassword()){
                messageBox.accessPointObj = arg0
                messageBox.accessPointEncryption = arg1
                messageBox.accessPointName = arg2
                messageBox.autoconnect = arg3

                autoConnectBox.checked = arg3

                if (!messageBox.visible)
                {
                    var connections = JSON.parse(dbusNetwork.connections)
                    for (var i = connections["wireless"].length - 1; i > 0; i -- )
                    {
                        if (connections["wireless"][i].Path === arg0)
                        {
                            uuid = connections["wireless"][i].Uuid
                            devicePath = connections["wireless"][i].Path
                        }
                    }

                    messageBox.showDialog()
                }

            }
        }
    }

    property string uuid :""
    property string devicePath : ""
    property bool oldCheckState: false

    property var dssLocale: DLocale {
        domain: "dde-control-center"
    }

    signal connect(string password)
    signal cancel()

    function dsTr(s){
        return dssLocale.dsTr(s)
    }

    function showDialog(){
        autoConnectBox.checked = getConnectionSession().GetKey("connection","autoconnect") == "true"?true:false
        oldCheckState = autoConnectBox.checked
        messageBox.visible = true
        messageBox.show()
    }

    function hideDialog(){
        messageBox.hide()
    }

    function cancelAction(){
        hideDialog()
        dbusNetwork.CancelSecret(accessPointObj, accessPointEncryption)
    }

    function connectAction(){
        hideDialog()
        dbusNetwork.FeedSecret(accessPointObj, accessPointEncryption, passwordInput.text, autoconnect)
    }


    Component {
        id: connectionSessionBuilder
        ConnectionSession {}
    }
    function getConnectionSession() {
        var connectionPath = dbusNetwork.EditConnection(uuid,devicePath)
        return connectionSessionBuilder.createObject(null, { path: connectionPath })
    }

    DWindowFrame {
        id: window
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent

            property int startX
            property int startY
            property bool holdFlag
            onPressed: {
                startX = mouse.x;
                startY = mouse.y;
                holdFlag = true;
            }
            onReleased: holdFlag = false;
            onPositionChanged: {
                if (holdFlag) {
                    messageBox.setX(messageBox.x + mouse.x - startX)
                    messageBox.setY(messageBox.y + mouse.y - startY)
                }
            }
        }


        DssH2{
            id: message
            anchors.left: passwordInput.left
            anchors.top: parent.top
            anchors.topMargin: 10

            text: dsTr('Please enter the password of  <font color="#faca57">%1</font>').arg(accessPointName)
            width: parent.width
            wrapMode: Text.WordWrap
        }

        DTextInput{
            id: passwordInput
            width: 310
            echoMode: TextInput.Password
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: message.bottom
            anchors.topMargin: 9

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 6
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width / 2
                height: parent.height
                color: "#636363"
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                text: dsTr("Password")
                visible: passwordInput.text == ""
            }
        }


        DCheckBox {
            id:autoConnectBox
            text:dsTr("Auto-connect")
            anchors.left: passwordInput.left
            anchors.top: passwordInput.bottom
            anchors.topMargin: 10
            onCheckedChanged: messageBox.autoconnect = checked
        }

        Row {
            id:buttonRow
            anchors.right: passwordInput.right
            anchors.top: autoConnectBox.bottom
            anchors.topMargin: 0
            spacing: 8

            DTextButton {
                text: dsTr("Cancel")
                onClicked: {
                    cancelAction()
                }
            }

            DTextButton {
                text: dsTr("Connect")
                onClicked: {
                    connectAction()
                }
            }
        }
    }
}
