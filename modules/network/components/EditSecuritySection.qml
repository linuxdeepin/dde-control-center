import QtQuick 2.1
import Deepin.Widgets 1.0
import "../widgets"

DBaseExpand{
    id: securitySettings
    property int activeExpandIndex: -1
    property int myIndex: 3
    property string sectionName: "Security"

    property var myKeys: connectionSessionObject.availableKeys[sectionName]
    property var errors: connectionSessionObject.errors[sectionName]

    onErrorsChanged: {
        for(var key in errors){
            print("==> [error] %1: %2".arg(key).arg(errors[key]))
        }
    }

    expanded: activeExpandIndex == myIndex
    onExpandedChanged: {
        if(header.item){
            header.item.active = expanded
        }
    }

    header.sourceComponent: DDownArrowHeader{
        text: dsTr("Security")
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
        DBaseLine {
            color: dconstants.contentBgColor
            leftMargin: contentLeftMargin
            leftLoader.sourceComponent: DssH2{
                text: dsTr("Security:")
            }

            rightLoader.sourceComponent: DComboBox{
                id: securityCombobox
                anchors.left: parent.left
                anchors.leftMargin: -3
                width: valueWidth
                text: getKey(securitySettings.sectionName, myKey)

                property string myKey: "vk-key-mgmt"
                property var menuLabels: connectionSessionObject.GetAvailableValues(securitySettings.sectionName, myKey)

                function menuSelect(i){
                    text = menuLabels[i]
                    setKey(securitySettings.sectionName, "Security", text)
                }

                onClicked: {
                    if(!rootMenu.visible){
                        var pos = mapToItem(null, 0, 0)
                        rootMenu.labels = securityCombobox.menuLabels
                        rootMenu.requestMenuItem = securityCombobox
                        rootMenu.posX = pos.x
                        rootMenu.posY = pos.y + height
                        rootMenu.innerWidth = width
                    }
                    rootMenu.visible = !rootMenu.visible
                }

                Component.onCompleted: {
                    print("psk:", getKey("Security", "psk"))
                    print("vk-key-mgmt:", connectionSessionObject.GetAvailableValues("Security","vk-key-mgmt"))
                }
            }
        }

        DBaseLine {
            color: dconstants.contentBgColor
            leftMargin: contentLeftMargin
            leftLoader.sourceComponent: DssH2{
                text: dsTr("Password:")
            }

            rightLoader.sourceComponent: DTextInput{
                width: valueWidth
                echoMode: showPasswordLine.rightLoader.item.showPassword? TextInput.Normal : TextInput.Password
                text: getKey(securitySettings.sectionName, "psk")
            }
        }

        DBaseLine {
            id: showPasswordLine
            rightLoader.sourceComponent: Row{
                width: valueWidth
                spacing: 6
                property bool showPassword: showPasswordButton.checked

                DSwitchButton{
                    id: showPasswordButton
                }

                DssH3{
                    text: dsTr("Show password")
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }
}
