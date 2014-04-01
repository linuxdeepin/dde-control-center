import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import QtQuick.Controls 1.1

Column{
    id: wirelessProperties
    width: parent.width

    property var uuid

    property int activeExpandIndex: 0
    property int valueWidth: 190
    property int contentLeftMargin: 18
    property var connectionSessionObject: {
        var connectionPath = nm.EditConnection(uuid)
        return connectionSession.createObject(wirelessProperties, { path: connectionPath })
    }

    DBaseLine{
        height: 38
        leftLoader.sourceComponent: Row{
            spacing: 6
            DssH2{
                text: dsTr("Connection Name:")
            }
            DTextInput{
                textInput.color: dconstants.fgColor
                text: connectionSessionObject.GetKey("General", "id")
            }
        }
    }

    DSeparatorHorizontal {}

    DBaseExpand{
        id: generalSettings
        property int myIndex: 0

        property string sectionName: "General"

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
                active: connectionSessionObject.GetKey(generalSettings.sectionName, "autoconnect")
            }

            DSwitchButtonHeader{
                color: dconstants.contentBgColor
                text: dsTr("All users may connect to this network")
                active: connectionSessionObject.GetKey(generalSettings.sectionName, "permissions")
            }
        }
    }

    DSeparatorHorizontal {}

    DBaseExpand{
        id: ipv4Settings
        property int myIndex: 1

        property string sectionName: "IPv4"

        expanded: activeExpandIndex == myIndex
        onExpandedChanged: {
            if(header.item){
                header.item.active = expanded
            }
        }

        header.sourceComponent: DDownArrowHeader{
            text: dsTr("IPv4")
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
                    text: dsTr("Method")
                }

                rightLoader.sourceComponent: DComboBox{
                    anchors.left: parent.left
                    anchors.leftMargin: -3
                    width: valueWidth
                    text: connectionSessionObject.GetKey(ipv4Settings.sectionName, "method")
                    onClicked: {
                        var pos = mapToItem(null, 0, 0)
                        rootMenu.posX = pos.x
                        rootMenu.posY = pos.y
                        rootMenu.innerWidth = width
                        rootMenu.visible = true
                    }
                }
            }

            DBaseLine {
                color: dconstants.contentBgColor
                leftMargin: contentLeftMargin
                leftLoader.sourceComponent: DssH2{
                    text: dsTr("IP Address")
                }

                rightLoader.sourceComponent: DTextInput{
                    width: valueWidth
                }
            }

            DBaseLine {
                color: dconstants.contentBgColor
                leftMargin: contentLeftMargin
                leftLoader.sourceComponent: DssH2{
                    text: dsTr("Netmask")
                }

                rightLoader.sourceComponent: DTextInput{
                    width: valueWidth
                }
            }

            DBaseLine {
                color: dconstants.contentBgColor
                leftMargin: contentLeftMargin
                leftLoader.sourceComponent: DssH2{
                    text: dsTr("Gateway")
                }

                rightLoader.sourceComponent: DTextInput{
                    width: valueWidth
                }
            }
            DBaseLine {
                color: dconstants.contentBgColor
                leftMargin: contentLeftMargin
                leftLoader.sourceComponent: DssH2{
                    text: dsTr("DNS Server")
                }

                rightLoader.sourceComponent: DTextInput{
                    width: valueWidth
                }
            }
        }
    }

    DSeparatorHorizontal {}

    DBaseExpand{
        id: securitySettings
        property int myIndex: 3

        property string sectionName: "Security"

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
                    anchors.left: parent.left
                    anchors.leftMargin: -3
                    width: valueWidth
                    text: "wep-key0"
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
                }
            }
        }
    }

    DSeparatorHorizontal{}

    DBaseLine{
        rightLoader.sourceComponent: Row {

            DTextButton{
                text: dsTr("Save")
            }

            DTextButton{
                text: dsTr("Close")
                onClicked: {
                    connectionSessionObject.Close()
                    stackView.pop(null)
                }
            }
        }
    }

}
