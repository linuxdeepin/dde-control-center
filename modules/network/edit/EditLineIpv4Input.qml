import QtQuick 2.1
import Deepin.Widgets 1.0
import "../widgets"

BaseEditLine {
    id: root
    
    property BaseEditLine previousIpv4Input
    property BaseEditLine nextIpv4Input
    property bool netmaskMode: false
    
    rightLoader.sourceComponent: EditKeyIpv4Input {
        width: valueWidth
        previousIpv4Input: root.previousIpv4Input
        nextIpv4Input: root.nextIpv4Input
        netmaskMode: root.netmaskMode
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
