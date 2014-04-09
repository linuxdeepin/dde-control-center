import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import QtQuick.Controls 1.1
import "widgets"

Column{
    id: wirelessProperties
    width: parent.width

    property var uuid

    property int activeExpandIndex: 1
    property int valueWidth: 190
    property int contentLeftMargin: 18
    property var connectionSessionObject: {
        var connectionPath = dbusNetwork.EditConnection(uuid)
        return connectionSession.createObject(wirelessProperties, { path: connectionPath })
    }

    function unmarshalJSON(valueJSON){
        var value = JSON.parse(valueJSON)
        return value
    }
    
    function marshalJSON(value){
        var valueJSON = JSON.stringify(value);
        return valueJSON
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
                text: unmarshalJSON(connectionSessionObject.GetKey("General", "id"))
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
                active: unmarshalJSON(connectionSessionObject.GetKey(generalSettings.sectionName, "autoconnect"))
            }

            DSwitchButtonHeader{
                color: dconstants.contentBgColor
                text: dsTr("All users may connect to this network")
                active: unmarshalJSON(connectionSessionObject.GetKey(generalSettings.sectionName, "permissions"))
            }
        }
    }

    DSeparatorHorizontal {}

    DBaseExpand{
        id: ipv4Settings
        property int myIndex: 1
        property string sectionName: "IPv4"
        property var myKeys: connectionSessionObject.availableKeys[sectionName]

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
                    id: ipv4MethodCombox
                    anchors.left: parent.left
                    width: valueWidth
                    text: unmarshalJSON(connectionSessionObject.GetKey(ipv4Settings.sectionName, "method"))

                    property var menuLabels: connectionSessionObject.GetAvailableValues(ipv4Settings.sectionName, "method")

                    function menuSelect(i){
                        text = menuLabels[i]
                        connectionSessionObject.SetKey(ipv4MethodCombox.sectionName, "method", marshalJSON(text))
                    }

                    onClicked: {
                        var pos = mapToItem(null, 0, 0)
                        rootMenu.labels = ipv4MethodCombox.menuLabels
                        rootMenu.requestMenuItem = ipv4MethodCombox
                        rootMenu.posX = pos.x
                        rootMenu.posY = pos.y + height
                        rootMenu.innerWidth = width
                        rootMenu.visible = true
                    }
                }
            }

            DBaseLine {
                id: ipAddressLine
                visible: getIndexFromArray("address", ipv4Settings.myKeys) != -1
                color: dconstants.contentBgColor
                leftMargin: contentLeftMargin
                leftLoader.sourceComponent: DssH2{
                    text: dsTr("IP Address")
                }

                rightLoader.sourceComponent: Ipv4Input{
                    width: valueWidth
                    onToNext: {
                        var ipAddress = getValue()
                        if(ipAddress){
                            var netmask = windowView.getDefaultMask(ipAddress)
                            if(netmask){
                                netmaskLine.rightLoader.item.setValue(netmask)
                            }
                        }
                        netmaskLine.rightLoader.item.getFocus()
                    }
                }
            }

            DBaseLine {
                id: netmaskLine
                visible: getIndexFromArray("address", ipv4Settings.myKeys) != -1
                color: dconstants.contentBgColor
                leftMargin: contentLeftMargin
                leftLoader.sourceComponent: DssH2{
                    text: dsTr("Netmask")
                }

                rightLoader.sourceComponent: Ipv4Input{
                    width: valueWidth
                    onToNext: {
                        gatewayLine.rightLoader.item.getFocus()
                    }
                }
            }

            DBaseLine {
                id: gatewayLine
                visible: getIndexFromArray("address", ipv4Settings.myKeys) != -1
                color: dconstants.contentBgColor
                leftMargin: contentLeftMargin
                leftLoader.sourceComponent: DssH2{
                    text: dsTr("Gateway")
                }

                rightLoader.sourceComponent: Ipv4Input{
                    width: valueWidth
                    onToNext: {
                        dnsServerLine.rightLoader.item.getFocus()
                    }
                }
            }

            DBaseLine {
                id: dnsServerLine
                color: dconstants.contentBgColor
                leftMargin: contentLeftMargin
                leftLoader.sourceComponent: DssH2{
                    text: dsTr("DNS Server")
                }

                rightLoader.sourceComponent: Ipv4Input{
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
            spacing: 6

            DTextButton{
                text: dsTr("Save")
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
