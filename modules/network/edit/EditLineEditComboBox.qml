import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    rightLoader.sourceComponent: EditKeyEditComboBox {
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
