/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Wanqing Yang
**
**  Author:     Wanqing Yang <yangwanqing@linuxdeepin.com>
**  Maintainer: Wanqing Yang <yangwanqing@linuxdeepin.com>
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


import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.Widgets 1.0
import Deepin.DockApplet 1.0
import Deepin.AppletWidgets 1.0
import DBus.Com.Deepin.Daemon.Network 1.0

DockApplet{
    id: wifiApplet
    title: "Wireless Network"
    appid: wirelessDevices[deviceIndex].Vendor

    icon: getIconUrl("wifi/ap-notconnect.png")

    property int xEdgePadding: 2
    property int titleSpacing: 10
    property int rootWidth: 180
    property int deviceIndex: {
        for (var i = 0; i < wirelessDevices.length; i++){
            if (wirelessDevices[i].Vendor == appletId){
                return i
            }
        }
        return 0
    }

    property var wirelessEnabled : dbusNetwork.IsDeviceEnabled(devicePath)
    property int deviceStatus: wirelessDevices[deviceIndex].State
    property string activeAp: wirelessDevices[deviceIndex].ActiveAp
    property string devicePath: wirelessDevices[deviceIndex].Path
    property string vendor: wirelessDevices[deviceIndex].Vendor
    property string deviceHwAddress: wirelessDevices[deviceIndex].HwAddress

    property var nmActiveConnections: unmarshalJSON(dbusNetwork.activeConnections)
    property var nmConnections: unmarshalJSON(dbusNetwork.connections)

    Connections {
        target: dbusNetwork
        onDeviceEnabled:{
            wirelessEnabled = dbusNetwork.IsDeviceEnabled(devicePath)
        }
    }

    function updateDockIcon() {
        if (!wirelessEnabled  || activeAp == "/")
        {
            wifiApplet.icon = getIconUrl("wifi/ap-notconnect.png")
            return
        }

        if (dbusNetwork.state != 70)
        {
            wifiApplet.icon = getIconUrl("wifi/ap-nonetwork.png")
            return
        }

        var apPower = 100
        for (var i = 0; i < accessPointsModel.count; i ++)
        {
            if (accessPointsModel.get(i).apPath == activeAp)
            {
                apPower = accessPointsModel.get(i).apSignal

                if (apPower < 5)
                    apPower = 0
                else if (apPower <= 25)
                    apPower = 25
                else if (apPower <= 50)
                    apPower = 50
                else if (apPower <= 75)
                    apPower = 75
                else
                    apPower = 100

                wifiApplet.icon = getIconUrl("wifi/ap-signal-%1.png".arg(apPower))

                return
            }
        }

        wifiApplet.icon = getIconUrl("wifi/ap-notconnect.png")
    }

    function unmarshalJSON(valueJSON) {
        if (!valueJSON) {
            print("==> [ERROR] unmarshalJSON", valueJSON)
        }
        var value = JSON.parse(valueJSON)
        return value
    }

    function showNetwork(id){
        dbusControlCenter.ShowModule("network")
    }

    function hideNetwork(id){
        setAppletState(false)
    }

    menu: AppletMenu{
        Component.onCompleted: {
            addItem(dsTr("_Run"), showNetwork);
            addItem(dsTr("_Undock"), hideNetwork);
        }
    }

    onActivate:{
        showNetwork(0)
    }

    window: DockQuickWindow {
        id: rootWindow
        width: rootWidth
        height: content.height + xEdgePadding * 2
        color: "transparent"

        onNativeWindowDestroyed: {
            toggleAppletState(appid)
            toggleAppletState(appid)
        }

        onQt5ScreenDestroyed: {
            console.log("Recive onQt5ScreenDestroyed")
            mainObject.restartDockApplet()
        }

        Item {
            width: parent.width
            height: content.height
            anchors.centerIn: parent

            Column {
                id: content
                width: parent.width
                spacing: 10

                DBaseLine {
                    height: 30
                    width: parent.width
                    color: "transparent"
                    leftLoader.sourceComponent: DssH2 {
                        elide:Text.ElideRight
                        width:80
                        text: wirelessDevices.length > 1 ? wirelessDevices[deviceIndex].Vendor : dsTr("Wireless Network")
                        color: "#ffffff"
                    }

                    rightLoader.sourceComponent: DSwitchButton {
                        id:wirelessSwitchButton
                        checked: wirelessEnabled
                        Connections{
                            target: wifiApplet
                            onWirelessEnabledChanged:{
                                wirelessSwitchButton.checked = wirelessEnabled
                            }
                        }
                        onClicked: dbusNetwork.EnableDevice(devicePath,checked)
                    }
                }

                Rectangle {
                    id:contantRec
                    width: rootWidth
                    height: apListView.height
                    visible: wirelessEnabled
                    color: "transparent"

                    Connections {
                        target: dbusNetwork
                        onAccessPointAdded:{
                            if(arg0 == devicePath){
                                print("onAccessPointAdded:", arg0, arg1)
                                var apObj = unmarshalJSON(arg1)
                                var index = accessPointsModel.getIndexByApPath(apObj.Path)
                                if(index == -1){
                                    var insertPosition = accessPointsModel.getInsertPosition(apObj)
                                    accessPointsModel.insert(insertPosition, {
                                        "apName": apObj.Ssid,
                                        "apSecured": apObj.Secured,
                                        "apSecuredInEap": apObj.SecuredInEap,
                                        "apSignal": apObj.Strength,
                                        "apPath": apObj.Path
                                    })
                                }

                                updateDockIcon()
                            }
                        }

                        onAccessPointRemoved: {
                            if(arg0 == devicePath){
                                print("onAccessPointRemoved:", arg0, arg1)
                                var apObj = unmarshalJSON(arg1)
                                var index = accessPointsModel.getIndexByApPath(apObj.Path)
                                if(index != -1){
                                    accessPointsModel.remove(index, 1)
                                }

                                updateDockIcon()
                            }
                        }

                        onAccessPointPropertiesChanged: {
                            if(arg0 == devicePath){
                                var apObj = unmarshalJSON(arg1)
                                var index = accessPointsModel.getIndexByApPath(apObj.Path)
                                if (index != -1){
                                    var apModelObj = accessPointsModel.get(index)
                                    apModelObj.apName = apObj.Ssid
                                    apModelObj.apSecured = apObj.Secured
                                    apModelObj.apSecuredInEap = apObj.SecuredInEap
                                    apModelObj.apSignal = apObj.Strength
                                    apModelObj.apPath = apObj.Path
                                }

                                updateDockIcon()
                            }
                        }
                    }

                    DConstants {
                        id:dContants
                    }

                    ListView {
                        id:apListView
                        width: parent.width
                        height: Math.min(childrenRect.height, 235)
                        model: accessPointsModel
                        delegate: WirelessApItem {}
                        visible: accessPointsModel.count > 0
                        clip: true

                        DScrollBar {
                            flickable: parent
                        }
                    }

                    ListModel {
                        id: accessPointsModel

                        function getIndexByApPath(path){
                            for(var i=0; i<count; i++){
                                var obj = get(i)
                                if(obj.apPath == path){
                                    return i
                                }
                            }
                            return -1
                        }

                        function getInsertPosition(apObj){
                            for(var i=0; i<count; i++){
                                var obj = get(i)
                                if(apObj.Path != obj.apPath && apObj.Strength >= obj.apSignal){
                                    return i
                                }
                            }
                            return count
                        }
                    }

                    Timer {
                        id: scanTimer
                        interval: 100
                        running: true
                        onTriggered: {
                            var accessPoints = unmarshalJSON(dbusNetwork.GetAccessPoints(devicePath))
                            accessPointsModel.clear()

                            for(var i in accessPoints){
                                // TODO ap
                                var apObj = accessPoints[i]
                                accessPointsModel.append({
                                    "apName": apObj.Ssid,
                                    "apSecured": apObj.Secured,
                                    "apSecuredInEap": apObj.SecuredInEap,
                                    "apSignal": apObj.Strength,
                                    "apPath": apObj.Path
                                })
                            }
                            contantRec.sortModel()
                            sortModelTimer.start()
                        }
                    }

                    Timer {
                        id: sortModelTimer
                        interval: 1000
                        repeat: true
                        onTriggered: {
                            contantRec.sortModel()
                        }
                    }


                    function sortModel()
                    {
                        var n;
                        var i;
                        for(n=0; n < accessPointsModel.count; n++){
                            for(i=n+1; i < accessPointsModel.count; i++){
                                if (accessPointsModel.get(n).apSignal < accessPointsModel.get(i).apSignal){
                                    accessPointsModel.move(i, n, 1);
                                    n=0; // Repeat at start since I can't swap items i and n
                                }
                            }
                        }
                    }
                }

                DssH3 {
                    text:vendor
                    width: parent.width
                    height: 18
                    visible: false
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
    }

}
