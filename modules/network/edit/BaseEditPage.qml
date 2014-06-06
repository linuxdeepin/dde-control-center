import QtQuick 2.1
import Deepin.Widgets 1.0

Column {
    id: editPage
    width: parent.width
    property int realHeight: childrenRect.height
    property int activeExpandIndex: -1
    property int valueWidth: 170
    property int contentLeftMargin: 18

    property var connectionSession
    property var availableSections: connectionSession.availableSections
    property var availableKeys:  connectionSession.availableKeys
    property var connectionData: connectionSession.data
    property var errors: connectionSession.errors
    property var connectionPath: connectionSession.connectionPath
    property var uuid: connectionSession.uuid

    function generalSetKey(section, key, value) {
        print("==> generalSetKey()", section, key, value) // TODO test
        connectionSession.SetKey(section, key, marshalJSON(value))
    }

    function generalGetKey(section, key) {
        return unmarshalJSON(connectionSession.GetKey(section, key))
    }

    function checkKeysInPage() {
        print("==> check all keys in page")
        doCheckKeysInPage(editPage)
    }
    function doCheckKeysInPage(parentItem) {
        if (parentItem.visible) {
            if (parentItem.objectName == "BaseEditLine") {
                parentItem.checkKey()
            }
        }
        for (var i=0; i<parentItem.children.length; i++) {
            var childItem = parentItem.children[i]
            doCheckKeysInPage(childItem)
        }
    }
    
    Component.onCompleted: {
        print("==> connection type:", connectionSession.type)
    }
}
