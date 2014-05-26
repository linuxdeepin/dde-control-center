import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseExpand{
    objectName: "BaseEditSection"
    
    property string section
    property int activeExpandIndex
    property int myIndex
    
    property var availableKeys: connectionSession.availableKeys[section]
    property var errors: connectionSession.errors[section]
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
    
    visible: getIndexFromArray(section, availableSections) != -1
    
    function updateKeysAlways() {
        if (!content.item) {
            // ignore if content is not ready
            return
        }
        for (var i=0; i<content.item.children.length; i++) {
            var objLine = content.item.children[i]
            if (objLine.objectName == "BaseEditLine" && objLine.visible && objLine.alwaysUpdate) {
                objLine.updateValue()
            }
        }
    }
    
    function checkKeys() {
        for (var i=0; i<content.item.children.length; i++) {
            var objLine = content.item.children[i]
            print(objLine)
            if (objLine.objectName == "BaseEditLine" && objLine.visible) {
                objLine.checkKey()
            }
        }
    }
    
    // TODO expand section if error occured when pressing save button
    function expandSection() {
        // expanded = true
    }
}
