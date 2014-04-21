import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditSection{
    id: securitySettings
    section: "security"

    header.sourceComponent: EditDownArrowHeader{
        text: dsTr("Security")
    }

    content.sourceComponent: Column {
        EditLineComboBox{
            key: "vk-key-mgmt"
            text: dsTr("Security:") // TODO section name append with ":"
        }
        EditLinePassword{
            id: passwordPsk
            key: "psk"
            text: dsTr("Password:")
        }
        EditLinePasswordToggle{
            targetTextInput: passwordPsk
        }
    }
}
