import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    id: wirelessDeviceList
    width: parent.width

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
            }
        }
    }

    ListView {
        width: parent.width
        height: Math.min(childrenRect.height, 255)
        model: accessPointsModel
        delegate: WirelessApItem {}
        visible: accessPointsModel.count > 0

        DScrollBar {
            flickable: parent
        }
    }

    DBaseLine{
        visible: accessPointsModel.count == 0
        color: "transparent"
        leftLoader.sourceComponent: DssH2{
            font.pixelSize: 12
            text: dsTr("Scanning...")
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
            sortModel()
            sortModelTimer.start()
        }
    }

    Timer {
        id: sortModelTimer
        interval: 1000
        repeat: true
        onTriggered: {
            sortModel()
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
