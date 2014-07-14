import QtQuick 2.1
import Deepin.Widgets 1.0
import "../edit"

BaseEditPage {
    id: rootPage
    width: parent.width

    DBaseLine {
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("New PPPoE Connection")
        }
    }
    DSeparatorHorizontal {}

    EditLineTextInput {
        id: lineConnectionId
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        errors: rootPage.errors
        section: "connection"
        key: "id"
        text: dsTr("Name")
    }
    
    // copy from ../edit_autogen/EditSectionPppoe.qml
    EditLineTextInput {
        id: linePppoeUsername
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        errors: rootPage.errors
        section: "pppoe"
        key: "username"
        text: dsTr("Username")
    }
    EditLineTextInput {
        id: linePppoeService
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        errors: rootPage.errors
        section: "pppoe"
        key: "service"
        text: dsTr("Service")
    }
    EditLinePasswordInput {
        id: linePppoePassword
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        errors: rootPage.errors
        section: "pppoe"
        key: "password"
        text: dsTr("Password")
    }
    DSeparatorHorizontal {}

    AddSectionSave {}
}
