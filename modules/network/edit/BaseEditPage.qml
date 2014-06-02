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
    property var data: connectionSession.data
    property var errors: connectionSession.errors
    property var availableSections: connectionSession.availableSections
    property var availableKeys:  connectionSession.availableKeys
    property var connectionPath: connectionSession.connectionPath
    property var uuid: connectionSession.uuid

    function checkKeysInPage() {
        print("==> check all keys in page")
        for (var i=0; i<children.length; i++) {
            var editSection = children[i]
            if (editSection.objectName == "BaseEditSection" && editSection.visible) {
                checkKeysInSection(editSection)
            }
        }
    }
    function checkKeysInSection(editSection) {
        print("==> check all keys in section", editSection.virtualSection)
        var childItems = editSection.content.item.children
        for (var i=0; i<childItems.length; i++) {
            var editLine = childItems[i]
            print("==>", editLine) // TODO test
            if (editLine.objectName == "BaseEditLine" && editLine.visible) {
                editLine.checkKey()
            }
        }
    }
    
    Component.onCompleted: {
        print("==> connection type:", connectionSession.type)
    }
}
