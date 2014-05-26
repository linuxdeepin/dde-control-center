import QtQuick 2.1
import Deepin.Widgets 1.0

Column{
    id: properiesPage
    width: parent.width
    property int realHeight: childrenRect.height

    // edit connection
    property var uuid
    property var devicePath: "/"
    property string connectionPath

    // create connection
    property bool create: false
    property string type

    // create connection for access point
    property bool createForAp: false
    property var apPath

    property int activeExpandIndex: -1
    property int valueWidth: 170
    property int contentLeftMargin: 18
    property var connectionSession: {
        if (create) {
            connectionPath = dbusNetwork.CreateConnection(type, devicePath)
            return connectionSessionBuilder.createObject(properiesPage, { path: connectionPath })
        } else if (createForAp) {
            connectionPath = dbusNetwork.CreateConnectionForAccessPoint(apPath, devicePath)
            return connectionSessionBuilder.createObject(properiesPage, { path: connectionPath })
        } else {
            connectionPath = dbusNetwork.EditConnection(uuid, devicePath)
            return connectionSessionBuilder.createObject(properiesPage, { path: connectionPath })
        }
    }
    property var availableSections: connectionSession.availablePages

    function generalSetKey(section, key, value){
        connectionSession.SetKey(section, key, marshalJSON(value))
    }

    function generalGetKey(section, key){
        return unmarshalJSON(connectionSession.GetKey(section, key))
    }

    function checkAllKeys() {
        print("check all keys")
        for (var i=0; i<children.length; i++) {
            var objSection = children[i]
            if (objSection.objectName == "BaseEditSection" && objSection.visible) {
                objSection.checkKeys()
            }
        }
    }
}
