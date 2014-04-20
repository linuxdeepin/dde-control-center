import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditSection{
    id: generalSettings
    sectionName: "General"
    
    header.sourceComponent: EditDownArrowHeader{
        text: dsTr("General")
    }

    content.sourceComponent: Column {
        EditLineSwitchButton{
            section: generalSettings.sectionName
            key: "autoconnect"
            text: dsTr("Automatically connect")
        }

        EditLineSwitchButton{
            section: generalSettings.sectionName
            key: "permissions"
            text: dsTr("All users may connect to this network")
        }
    }
}
