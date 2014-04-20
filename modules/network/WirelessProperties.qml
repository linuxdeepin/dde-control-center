import QtQuick 2.1
import Deepin.Widgets 1.0
import "components"
// import "components_autogen"

BaseEditPage {
    id: wirelessProperties
    activeExpandIndex: 0

    EditTitle {}

    DSeparatorHorizontal {}
    EditSectionGeneral{
        id: generalSettings
        myIndex: 0
        activeExpandIndex: wirelessProperties.activeExpandIndex
    }

    DSeparatorHorizontal {}
    EditSectionIpv4{
        id: ipv4Settings
        myIndex: 1
        activeExpandIndex: wirelessProperties.activeExpandIndex
    }

    DSeparatorHorizontal {}
    EditSectionSecurity {
        id: securitySettings    // TODO
        myIndex: 2
        activeExpandIndex: wirelessProperties.activeExpandIndex
    }

    DSeparatorHorizontal{}
    DBaseLine{
        rightLoader.sourceComponent: Row {
            spacing: 6

            DTextButton{
                text: dsTr("Save")
                onClicked: {
                    // save all keys TODO
                    generalSettings.saveKeys()
                    securitySettings.saveKeys()
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
