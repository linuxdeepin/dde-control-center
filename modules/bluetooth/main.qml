import QtQuick 2.1
import Deepin.Widgets 1.0

Column {
    id: bluetooth
    anchors.fill: parent

    DssTitle {
        text: dsTr("Bluetooth")
        width: parent.width
    }

    DSeparatorHorizontal {}

    DBaseLine {
        height: 38
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("ON/OFF")
        }
        rightLoader.sourceComponent: DSwitchButton {

        }
    }

    DSeparatorHorizontal {}

    DBaseLine {
        height: 38
        leftLoader.sourceComponent: TextWithHint {
            text: "ShowDevice"
            hint: "120s"
            textColor: dconstants.fgColor
            hintColor: dconstants.fgDarkColor
        }
        rightLoader.sourceComponent: DRadioButton {
            buttonModel: [
                {"buttonId": "visible", "buttonLabel": dsTr("Visible")},
                {"buttonId": "invisible", "buttonLabel": dsTr("Invisible")},
            ]

            initializeIndex: 0
        }
    }

    DSeparatorHorizontal {}

    DBaseLine {
        height: 38
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("Device name")
        }
        rightLoader.sourceComponent: DTextInput {
            text: "Toshiba"
        }
    }

    DSeparatorHorizontal {}

    DBaseExpand {
        header.sourceComponent: DBaseLine {
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Devices nearby")
            }
            rightLoader.sourceComponent: RefreshButton {
            }
        }
    }

    DSeparatorHorizontal {}

    DBaseExpand {
        id: recently_expand
        
        header.sourceComponent: DDownArrowHeader {
            text: dsTr("Recently connected devices")
            onActiveChanged: recently_expand.expanded = active
        }
    }

    DSeparatorHorizontal {}
}
