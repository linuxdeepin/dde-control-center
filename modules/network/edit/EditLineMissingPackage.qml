import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    // TODO clickable pacakge text, and jump to software center
    
    leftLoader.sourceComponent: DssH2{
        text: {
            if (root.text) {
                return root.text
            } else {
                return dsTr("Need Pacakge")
            }
        }
    }
    
    rightLoader.sourceComponent: EditKeyMissingPackage {
        width: valueWidth
        connectionSession: root.connectionSession
        availableSections: root.availableSections
        availableKeys: root.availableKeys
        connectionData: root.connectionData
        errors: root.errors
        section: root.section
        key: root.key
        alwaysUpdate: root.alwaysUpdate
    }
}    

