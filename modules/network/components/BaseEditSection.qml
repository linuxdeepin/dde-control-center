import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseExpand{
    property int activeExpandIndex
    property int myIndex
    property string sectionName
    
    property var availableKeys: connectionSessionObject.availableKeys[sectionName]
    function isKeyAvailable(key) {
        return getIndexFromArray(key, availableKeys) != -1
    }
    
    property var errors: connectionSessionObject.errors[sectionName]
    onErrorsChanged: {
        for(var key in errors){
            print("==> [error] %1: %2".arg(key).arg(errors[key]))
        }
    }
    function isValueError(key) {
        return errors[root.key] ? true : false
    }

    expanded: activeExpandIndex == myIndex
    onExpandedChanged: {
        if(header.item){
            header.item.active = expanded
        }
    }
    
    function saveKeys() {
        // TODO
        print("saveKeys generalSettings")
    }
}
