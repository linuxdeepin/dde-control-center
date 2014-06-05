import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    property int min: 0
    property int max: 65535
    
    rightLoader.sourceComponent: EditKeySpinner {
        width: valueWidth
        min: root.min
        max: root.max
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
