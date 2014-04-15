import QtQuick 2.1
import Deepin.Widgets 1.0
import "components"

BaseEditPage {
    id: wirelessProperties
    activeExpandIndex: 1

    EditTitle {}

    DSeparatorHorizontal {}

    DBaseExpand{
        id: generalSettings
        property int myIndex: 0
        property string sectionName: "General"
        property var myKeys: connectionSessionObject.availableKeys[sectionName]
        property var errors: connectionSessionObject.errors[sectionName]

        expanded: activeExpandIndex == myIndex
        onExpandedChanged: {
            if(header.item){
                header.item.active = expanded
            }
        }

        header.sourceComponent: DDownArrowHeader{
            text: dsTr("General")
            onClicked: {
                if(activeExpandIndex == root.myIndex){
                    activeExpandIndex = -1
                }
                else{
                    activeExpandIndex = root.myIndex
                }
            }
            Component.onCompleted: {
                active = (activeExpandIndex == root.myIndex)
            }
        }

        content.sourceComponent: Column {
            DSwitchButtonHeader{
                color: dconstants.contentBgColor
                text: dsTr("Automatically connect")
                active: getKey(generalSettings.sectionName, "autoconnect")
            }

            DSwitchButtonHeader{
                color: dconstants.contentBgColor
                text: dsTr("All users may connect to this network")
                active: getKey(generalSettings.sectionName, "permissions")
            }
        }
    }

    DSeparatorHorizontal {}

    EditIpv4Section{
        activeExpandIndex: wirelessProperties.activeExpandIndex
    }

    DSeparatorHorizontal {}

    EditSecuritySection {
        activeExpandIndex: wirelessProperties.activeExpandIndex
    }

    DSeparatorHorizontal{}

    DBaseLine{
        rightLoader.sourceComponent: Row {
            spacing: 6

            DTextButton{
                text: dsTr("Save")
                onClicked: {
                    if (connectionSessionObject.Save()){
                        stackView.reset()
                    }
                }
            }

            DTextButton{
                text: dsTr("Close")
                onClicked: {
                    connectionSessionObject.Close()
                    stackView.reset()
                }
            }
        }
    }

}
