import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseExpand{
    id: editSection
    objectName: "BaseEditSection"
    
    property var connectionSession
    property var data
    property var errors
    property var availableSections
    property var availableKeys
    property string virtualSection
    property var availableVirtualSections: connectionSession.availableVirtualSections
    visible: getIndexFromArray(virtualSection, availableVirtualSections) != -1
    
    // TODO remove
    // property var errors: connectionSession.errors[section]
    // onErrorsChanged: {
    //     for(var key in errors){
    //         print("==> [error] %1: %2".arg(key).arg(errors[key]))
    //     }
    // }

    property int activeExpandIndex
    property int myIndex
    expanded: activeExpandIndex == myIndex
    onExpandedChanged: {
        if(header.item){
            header.item.active = expanded
        }
    }
    // TODO expand section if error occured after save button pressed
    function expandSection() {
        activeExpandIndex = myIndex
        // expanded = true
    }
    
    // TODO remove
    // function updateKeysAlways() {
    //     if (!content.item) {
    //         // ignore if content is not ready
    //         return
    //     }
    //     for (var i=0; i<content.item.children.length; i++) {
    //         var editLine = content.item.children[i]
    //         if (editLine.objectName == "BaseEditLine" && editLine.visible && editLine.alwaysUpdate) {
    //             editLine.updateValue()
    //         }
    //     }
    // }
    
    // function checkKeys() {
    //     for (var i=0; i<content.item.children.length; i++) {
    //         var editLine = content.item.children[i]
    //         print("==>", editLine) // TODO test
    //         if (editLine.objectName == "BaseEditLine" && editLine.visible) {
    //             editLine.checkKey()
    //         }
    //     }
    // }
}
