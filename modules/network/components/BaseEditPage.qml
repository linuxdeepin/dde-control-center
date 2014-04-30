import QtQuick 2.1
import Deepin.Widgets 1.0

Column{
    id: properiesPage
    width: parent.width

    // edit connection
    property var uuid
    property var devicePath
    
    // create connection
    property bool create: false
    property string type

    property int activeExpandIndex: -1
    property int valueWidth: 170
    property int contentLeftMargin: 18
    property var connectionSessionObject: {
        if (create) {
            var connectionPath = dbusNetwork.CreateConnection(type, "/")
            return connectionSession.createObject(properiesPage, { path: connectionPath })
        } else {
            var connectionPath = dbusNetwork.EditConnection(uuid, devicePath)
            return connectionSession.createObject(properiesPage, { path: connectionPath })
        }
    }
    property var availableSections: connectionSessionObject.availablePages

    function generalSetKey(section, key, value){
        connectionSessionObject.SetKey(section, key, marshalJSON(value))
    }

    function generalGetKey(section, key){
        // print("--> generalGetKey", section, key) // TODO test
        // var value = unmarshalJSON(connectionSessionObject.GetKey(section, key))
        // print("--| generalGetKey", section, key, value)
        return unmarshalJSON(connectionSessionObject.GetKey(section, key))
    }

    function marshalJSON(value){
        var valueJSON = JSON.stringify(value);
        // TODO test
        // print("==> unmarshalJSON:", value, valueJSON)
        return valueJSON
    }
    
    function unmarshalJSON(valueJSON){
        // TODO test
        // print("==> unmarshalJSON:", valueJSON)
        var value = JSON.parse(valueJSON)
        return value
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
    
    // TODO
    Component.onCompleted: {
        print("BaseEditPage.availableSections:", availableSections)
    }
}
