import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseExpand{
    objectName: "BaseEditSection"
    
    property string section
    property int activeExpandIndex
    property int myIndex
    
    // TODO fix property undefined issue after press save button
    property var availableKeys: connectionSessionObject.availableKeys[section]
    // property var availableKeys
    // Binding on availableKeys {
    //     when: section != undefined
    //     value: connectionSessionObject.availableKeys[section]
    // }
    
    property var errors: connectionSessionObject.errors[section]
    // property var errors
    // Binding on errors {
    //     when: section != undefined
    //     value: connectionSessionObject.errors[section]
    // }
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
    
    visible: getIndexFromArray(section, avaiableSections) != -1
    
    function saveKeys() {
        for (var i=0; i<content.item.children.length; i++) {
            var objLine = content.item.children[i]
            if (objLine.objectName == "BaseEditLine" && objLine.visible) {
                objLine.saveKey()
            }
        }
    }
}
