import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditSection{
    id: securitySettings
    sectionName: "Security"

    header.sourceComponent: EditDownArrowHeader{
        text: dsTr("Security")
    }

    content.sourceComponent: Column {
        EditLineComboBox {
            section: securitySettings.sectionName
            key: "vk-key-mgmt"
            text: dsTr("Security:")
        }
        EditLinePassword {
            id: passwordPsk
            section: securitySettings.sectionName
            key: "psk"
            text: dsTr("Password:")
        }
        EditLinePasswordToggle{
            targetTextInput: passwordPsk
        }
    }
}
