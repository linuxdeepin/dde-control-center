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
}
