import QtQuick 2.1
import Deepin.Widgets 1.0
import "../widgets"

DBaseExpand {
    id: ipv4Settings
    property int activeExpandIndex: -1
    property int myIndex: 1
    property string sectionName: "IPv4"

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
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Method")
            }

            rightLoader.sourceComponent: DComboBox{
                id: ipv4MethodCombox
                anchors.left: parent.left
                width: valueWidth
                text: getKey(ipv4Settings.sectionName, "method")

                property var menuLabels: connectionSessionObject.GetAvailableValues(ipv4Settings.sectionName, "method")

                function menuSelect(i){
                    text = menuLabels[i]
                    setKey(ipv4Settings.sectionName, "method", text)
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
            property string keyName: "vk-addresses-address"
            visible: getIndexFromArray(keyName, ipv4Settings.myKeys) != -1
            color: dconstants.contentBgColor
            leftMargin: contentLeftMargin
            leftLoader.sourceComponent: DssH2{
                text: dsTr("IP Address")
            }

            rightLoader.sourceComponent: Ipv4Input{
                isError: ipv4Settings.errors[ipAddressLine.keyName] ? true : false
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
                onIsFocusChanged: {
                    if(!isFocus){
                        var ipAddress = getValue()
                        setKey(ipv4Settings.sectionName, ipAddressLine.keyName, ipAddress)
                    }
                }
                
                Component.onCompleted: {
                    if(ipAddressLine.visible){
                        var value = getKey(ipv4Settings.sectionName, ipAddressLine.keyName)
                        if(value){
                            setValue(value)
                        }
                    }
                }
            }
        }

        DBaseLine {
            id: netmaskLine
            property string keyName: "vk-addresses-mask"
            visible: getIndexFromArray(keyName, ipv4Settings.myKeys) != -1
            color: dconstants.contentBgColor
            leftMargin: contentLeftMargin
            leftLoader.sourceComponent: DssH2{
                text: dsTr("Netmask")
            }

            rightLoader.sourceComponent: Ipv4Input{
                isError: ipv4Settings.errors[netmaskLine.keyName] ? true : false
                width: valueWidth
                onToNext: {
                    gatewayLine.rightLoader.item.getFocus()
                }
                onIsFocusChanged: {
                    if(!isFocus){
                        var value = getValue()
                        setKey(ipv4Settings.sectionName, netmaskLine.keyName, value)
                    }
                }
                Component.onCompleted: {
                    if(netmaskLine.visible){
                        var value = getKey(ipv4Settings.sectionName, netmaskLine.keyName)
                        if(value){
                            setValue(value)
                        }
                    }
                }
            }
        }

        DBaseLine {
            id: gatewayLine
            property string keyName: "vk-addresses-gateway"
            visible: getIndexFromArray(keyName, ipv4Settings.myKeys) != -1
            color: dconstants.contentBgColor
            leftMargin: contentLeftMargin
            leftLoader.sourceComponent: DssH2{
                text: dsTr("Gateway")
            }

            rightLoader.sourceComponent: Ipv4Input{
                isError: ipv4Settings.errors[gatewayLine.keyName] ? true : false
                width: valueWidth
                onToNext: {
                    dnsServerLine.rightLoader.item.getFocus()
                }
                onIsFocusChanged: {
                    if(!isFocus){
                        var value = getValue()
                        setKey(ipv4Settings.sectionName, gatewayLine.keyName, value)
                    }
                }
                Component.onCompleted: {
                    if(gatewayLine.visible){
                        var value = getKey(ipv4Settings.sectionName, gatewayLine.keyName)
                        if(value){
                            setValue(value)
                        }
                    }
                }
            }
        }

        DBaseLine {
            id: dnsServerLine
            property string keyName: "vk-dns"
            color: dconstants.contentBgColor
            leftMargin: contentLeftMargin
            leftLoader.sourceComponent: DssH2{
                text: dsTr("DNS Server")
            }

            rightLoader.sourceComponent: Ipv4Input{
                isError: ipv4Settings.errors[dnsServerLine.keyName] ? true : false
                width: valueWidth
                onIsFocusChanged: {
                    if(!isFocus){
                        var value = getValue()
                        setKey(ipv4Settings.sectionName, dnsServerLine.keyName, value)
                    }
                }

                Component.onCompleted: {
                    if(dnsServerLine.visible){
                        var value = getKey(ipv4Settings.sectionName, dnsServerLine.keyName)
                        if(value){
                            setValue(value)
                        }
                    }
                }

            }
        }
    }
}

