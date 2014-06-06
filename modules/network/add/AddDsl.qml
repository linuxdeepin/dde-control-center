import QtQuick 2.1
import Deepin.Widgets 1.0
import "../edit"

BaseEditPage {
    id: addPage
    width: parent.width

    DBaseLine {
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("New PPPoE Connection")
        }
    }
    DSeparatorHorizontal {}

    EditLineTextInput {
        id: lineConnectionId
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "connection"
        key: "id"
        text: dsTr("Name")
    }
    
    // copy from ../edit_autogen/EditSectionPppoe.qml
    EditLineTextInput {
        id: linePppoeUsername
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "pppoe"
        key: "username"
        text: dsTr("Username")
    }
    EditLineTextInput {
        id: linePppoeService
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "pppoe"
        key: "service"
        text: dsTr("Service")
    }
    EditLinePasswordInput {
        id: linePppoePassword
        connectionSession: addPage.connectionSession
        availableSections: addPage.availableSections
        availableKeys: addPage.availableKeys
        connectionData: addPage.connectionData
        errors: addPage.errors
        section: "pppoe"
        key: "password"
        text: dsTr("Password")
    }
    DSeparatorHorizontal {}

    AddSectionSave {}
}
