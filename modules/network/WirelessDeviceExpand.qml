import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0


DBaseExpand {
    id: wirelessDevicesExpand
    width: parent.width

    property string devicePath: "/"
    property string deviceHwAddr
    property int deviceStatus: 0

    property string inConnectingApPath: "/"

    Component.onCompleted: {
        for(var i in wirelessDevices){
            if(wirelessDevices[i].Path == wirelessDevicesExpand.devicePath){
                wirelessDevicesExpand.deviceStatus = wirelessDevices[i].State
            }
        }
        if(!scanTimer.running){
            scanTimer.start()
        }
    }

    ListModel {
        id: accessPointsModel

        function getIndexByApPath(path){
            for(var i; i<count; i++){
                var obj = get(i)
                if(obj.apPath == path){
                    return i
                }
            }
            return -1
        }

        function getInsertPosition(apObj){
            var position = count
            for(var i; i<count; i++){
                var obj = get(i)
                if(apObj.Path != obj.apPath && apObj.Strength >= obj.apSignal){
                    position = i
                    break
                }
            }
            return position
        }
    }

    Connections{
        target: dbusNetwork
        onAccessPointAdded:{
            if(arg0 == devicePath){
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
            }
        }

        onAccessPointRemoved:{
            if(arg0 == devicePath){
                var apObj = unmarshalJSON(arg1)
                var index = accessPointsModel.getIndexByApPath(apObj.Path)
                if(index != -1){
                    accessPointsModel.remove(index, 1)
                }
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

                    var insertPosition = accessPointsModel.getInsertPosition(apObj)
                    if(insertPosition != index){
                        accessPointsModel.move(index, position, 1)
                    }
                }
            }
        }

        onDeviceStateChanged: {
            if(arg0 == devicePath){
                wirelessDevicesExpand.deviceStatus = arg1
                if(arg1 == 100){ // TODO
                    wirelessDevicesExpand.inConnectingApPath = "/"
                }
            }
        }
    }

    expanded: deviceStatus != 20 // TODO
    header.sourceComponent: DBaseLine{

        leftLoader.sourceComponent: DssH2 {
            anchors.verticalCenter: parent.verticalCenter
            text: {
                if(wirelessDevices.length < 2){
                    return dsTr("Wireless Network")
                }
                else{
                    return dsTr("Wireless Network %1").arg(index + 1).arg(deviceStatus)
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: scanTimer.restart()
            }
        }

        rightLoader.sourceComponent: DSwitchButton{
            checked: wirelessDevicesExpand.expanded
            onClicked: {
                dbusNetwork.wirelessEnabled = checked
                accessPointsModel.clear()
            }
        }
    }

    content.sourceComponent: Column {
        width: parent.width

        ListView {
            width: parent.width
            height: childrenRect.height
            boundsBehavior: Flickable.StopAtBounds
            model: accessPointsModel
            delegate: WirelessItem {
                devicePath: wirelessDevicesExpand.devicePath
                deviceHwAddr: wirelessDevicesExpand.deviceHwAddr
            }
        }

        DBaseLine {
            id: hiddenNetworkLine
            color: dconstants.contentBgColor

            property bool hovered: false

            leftLoader.sourceComponent: DssH2 {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                text: dsTr("Connect Hidden Access Point")
                font.pixelSize: 12
                color: hiddenNetworkLine.hovered ? dconstants.hoverColor : dconstants.fgColor
            }
            rightLoader.sourceComponent: DArrowButton {
                onClicked: goToConnectHiddenAP()
            }

            MouseArea {
                z: -1
                anchors.fill:parent
                hoverEnabled: true
                onEntered: parent.hovered = true
                onExited: parent.hovered = false
                onClicked: goToConnectHiddenAP()
            }
        }

        DSeparatorHorizontal {}

        DBaseLine {
            id: createApLine
            color: dconstants.contentBgColor

            property bool hovered: false

            leftLoader.sourceComponent: DssH2 {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                text: dsTr("Create Access Point")
                font.pixelSize: 12
                color: createApLine.hovered ? dconstants.hoverColor : dconstants.fgColor
            }
            rightLoader.sourceComponent: DArrowButton {
                onClicked: goToCreateAP()
            }

            MouseArea {
                z: -1
                anchors.fill:parent
                hoverEnabled: true
                onEntered: parent.hovered = true
                onExited: parent.hovered = false
                onClicked: goToCreateAP()
            }
        }
    }

    function goToConnectHiddenAP(){
    }

    function goToCreateAP(){
    }

    function sortModel()
    {
        var n;
        var i;
        for (n=0; n < accessPointsModel.count; n++){
            for (i=n+1; i < accessPointsModel.count; i++)
            {
                if (accessPointsModel.get(n).apSignal < accessPointsModel.get(i).apSignal)
                {
                    accessPointsModel.move(i, n, 1);
                    n=0; // Repeat at start since I can't swap items i and n
                }
            }
        }
    }

    Timer {
        id: scanTimer
        interval: 100
        onTriggered: {
            var accessPoints = unmarshalJSON(dbusNetwork.GetAccessPoints(devicePath))
            wirelessDevicesExpand.inConnectingApPath = "/"
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
            wirelessDevicesExpand.sortModel()
        }
    }
}
