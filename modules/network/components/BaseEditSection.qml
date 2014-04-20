import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseExpand{
    property string section
    property int activeExpandIndex
    property int myIndex
    
    property var availableKeys: connectionSessionObject.availableKeys[section]
    
    property var errors: connectionSessionObject.errors[section]
    onErrorsChanged: {
        for(var key in errors){
            print("==> [error] %1: %2".arg(key).arg(errors[key]))
        }
    }

    expanded: activeExpandIndex == myIndex
    onExpandedChanged: {
        if(header.item){
            header.item.active = expanded
        }
    }
    
    // TODO
    // function saveKeys() {
        // print("saveKeys generalSettings")
    // }
}
