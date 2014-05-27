import QtQuick 2.1
import Deepin.Widgets 1.0

Column {
    id: propertiesPage
    width: parent.width
    property int realHeight: childrenRect.height
    property int activeExpandIndex: -1
    property int valueWidth: 170
    property int contentLeftMargin: 18

    property var connectionSession
    property var connectionPath: connectionSession.connectionPath
    property var uuid: connectionSession.uuid
    property var availableSections: connectionSession.availablePages

    function generalSetKey(section, key, value) {
        connectionSession.SetKey(section, key, marshalJSON(value))
    }

    function generalGetKey(section, key) {
        return unmarshalJSON(connectionSession.GetKey(section, key))
    }

    function checkAllKeys() {
        print("==> check all keys")
        for (var i=0; i<children.length; i++) {
            var objSection = children[i]
            if (objSection.objectName == "BaseEditSection" && objSection.visible) {
                objSection.checkKeys()
            }
        }
    }
}
