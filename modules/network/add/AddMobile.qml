import QtQuick 2.1
import Deepin.Widgets 1.0
import "../edit"

BaseEditPage {
    id: rootPage
    width: parent.width

    Component.onCompleted: {
        // test code
        // lineVsMobileVkMobileServiceType.setKey(nmConnectionTypeMobileGsm)
        // lineGsmApn.setKey()
        // lineGsmNumber.setKey()
        // lineGsmUsername.setKey()
        // lineGsmPassword.setKey()
    }
    
    DBaseLine {
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("New Mobile Connection")
        }
    }
    DSeparatorHorizontal {}

    EditLineTextInput {
        id: lineConnectionId
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        connectionData: rootPage.connectionData
        errors: rootPage.errors
        section: "connection"
        key: "id"
        text: dsTr("Name")
    }
    
    // copy from ../edit_autogen/EditSectionMobile.qml
    EditLineComboBox {
        id: lineVsMobileVkMobileServiceType
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        connectionData: rootPage.connectionData
        errors: rootPage.errors
        section: "vs-mobile"
        key: "vk-mobile-service-type"
        text: dsTr("Service Type")
    }
    EditLineEditComboBox {
        id: lineGsmApn
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        connectionData: rootPage.connectionData
        errors: rootPage.errors
        section: "gsm"
        key: "apn"
        text: dsTr("APN")
    }
    EditLineTextInput {
        id: lineGsmNumber
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        connectionData: rootPage.connectionData
        errors: rootPage.errors
        section: "gsm"
        key: "number"
        text: dsTr("Number")
    }
    EditLineTextInput {
        id: lineGsmUsername
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        connectionData: rootPage.connectionData
        errors: rootPage.errors
        section: "gsm"
        key: "username"
        text: dsTr("Username")
    }
    EditLinePasswordInput {
        id: lineGsmPassword
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        connectionData: rootPage.connectionData
        errors: rootPage.errors
        section: "gsm"
        key: "password"
        text: dsTr("Password")
    }
    EditLineTextInput {
        id: lineCdmaNumber
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        connectionData: rootPage.connectionData
        errors: rootPage.errors
        section: "cdma"
        key: "number"
        text: dsTr("Number")
    }
    EditLineTextInput {
        id: lineCdmaUsername
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        connectionData: rootPage.connectionData
        errors: rootPage.errors
        section: "cdma"
        key: "username"
        text: dsTr("Username")
    }
    EditLinePasswordInput {
        id: lineCdmaPassword
        connectionSession: rootPage.connectionSession
        availableSections: rootPage.availableSections
        availableKeys: rootPage.availableKeys
        connectionData: rootPage.connectionData
        errors: rootPage.errors
        section: "cdma"
        key: "password"
        text: dsTr("Password")
    }
    DSeparatorHorizontal {}

    AddSectionSave {}
}
