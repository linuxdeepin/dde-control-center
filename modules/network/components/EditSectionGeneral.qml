import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditSection{
    id: generalSettings
    section: "General"
    
    header.sourceComponent: EditDownArrowHeader{
        text: dsTr("General")
    }

    content.sourceComponent: Column {
        EditLineSwitchButton{
            key: "autoconnect"
            text: dsTr("Automatically connect")
        }
        // TODO remove
        EditLineSwitchButton{
            key: "permissions"
            text: dsTr("All users may connect to this network")
        }
    }
}
