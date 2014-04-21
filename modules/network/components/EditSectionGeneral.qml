import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditSection{
    id: generalSettings
    section: "general"
    
    header.sourceComponent: EditDownArrowHeader{
        text: dsTr("General")
    }

    content.sourceComponent: Column {
        EditLineSwitchButton{
            key: "autoconnect"
            text: dsTr("Automatically connect")
        }
    }
}
