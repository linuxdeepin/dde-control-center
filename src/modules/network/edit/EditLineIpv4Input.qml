import QtQuick 2.1
import Deepin.Widgets 1.0
import "../widgets"

BaseEditLine {
    id: root
    setKeyAlways: true
    
    property BaseEditLine previousIpv4Input
    property BaseEditLine nextIpv4Input
    property bool netmaskMode: false
    
    rightLoader.sourceComponent: Ipv4Input {
        // activeFocusOnTab: true  // TODO
        width: valueWidth
        isError: root.showError
        
        onIsFocusChanged: {
            if (!isFocus) {
                root.showErrorConditon = true
            }
        }
        
        onTextChanged: {
            setKey(getValue())
        }
        
        onToPrevious: {
            if (previousIpv4Input && previousIpv4Input.visible) {
                previousIpv4Input.rightLoader.item.getFocus()
            }
        }
        onToNext: {
            if (nextIpv4Input && nextIpv4Input.visible) {
                var ipAddress = getValue()
                if (ipAddress) {
                    if (nextIpv4Input.netmaskMode) {
                        // TODO fix netmask
                        // var netmask = mainObject.getDefaultMask(ipAddress)
                        var netmask = "255.255.255.0"
                        if (netmask != "") {
                            nextIpv4Input.rightLoader.item.setValue(netmask)
                        }
                    }
                }
                nextIpv4Input.rightLoader.item.getFocus()
            }
        }
        
        Connections {
            target: root
            onWidgetShown: {
                if (root.visible && root.cacheValue) {
                    setValue(root.cacheValue)
                }
            }
            onCacheValueChanged: {
                if (root.visible && root.cacheValue) {
                    setValue(root.cacheValue)
                }
            }
        }
    }
}
