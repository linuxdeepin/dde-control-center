import QtQuick 2.1
import Deepin.Widgets 1.0

Column{
    id: properiesPage
    width: parent.width

    property var uuid
    property var devicePath

    property int activeExpandIndex: -1
    property int valueWidth: 190
    property int contentLeftMargin: 18
    property var connectionSessionObject: {
        var connectionPath = dbusNetwork.EditConnection(uuid, devicePath)
        return connectionSession.createObject(properiesPage, { path: connectionPath })
    }

    // TODO
    function saveAllKeys() {
    }
    
    function generalSetKey(section, key, value){
        connectionSessionObject.SetKey(section, key, marshalJSON(value))
    }

    function generalGetKey(section, key){
        return unmarshalJSON(connectionSessionObject.GetKey(section, key))
    }

    function marshalJSON(value){
        var valueJSON = JSON.stringify(value);
        return valueJSON
    }
    
    function unmarshalJSON(valueJSON){
        var value = JSON.parse(valueJSON)
        return value
    }
}
