import QtQuick 2.1
import Deepin.Widgets 1.0
import "../widgets"

BaseEditLine {
    id: root
    
    property BaseEditLine previousIpv4Input
    property BaseEditLine nextIpv4Input
    property bool netmaskMode: false
    
    rightLoader.sourceComponent: Ipv4Input{
        width: valueWidth
        isError: root.showError
        
        onIsFocusChanged: {
            if(!isFocus){
                root.showErrorConditon = true
            }
        }
        
        onTextChanged: {
            root.value = getValue()
            setKey()
        }
        
        onToPrevious: {
            if (previousIpv4Input && previousIpv4Input.visible) {
                previousIpv4Input.rightLoader.item.getFocus()
            }
        }
        onToNext: {
            if (nextIpv4Input && nextIpv4Input.visible) {
                var ipAddress = getValue()
                if(ipAddress){
                    if (nextIpv4Input.netmaskMode) {
                        // TODO fix netmas
                        // var netmask = windowView.getDefaultMask(ipAddress)
                        var netmask = "255.255.255.0"
                        if (netmask) {
                            nextIpv4Input.rightLoader.item.setValue(netmask)
                        }
                    }
                }
                nextIpv4Input.rightLoader.item.getFocus()
            }
        }
        
        // make onVisibleChanged() wrapped by Connections to ensure
        // code excuted after EditLineIpv4Input.onVisibleChanged()
        Connections {
            target: root
            onVisibleChanged: {
                if (root.visible) {
                    if (root.value) {
                        setValue(root.value)
                    }
                }
                // TODO test
                print("EditLineIpv4Input.onVisibleChanged", visible ? "(show)" : "(hide)", section, key, getValue())
            }
        }
        
        // Component.onCompleted() is still need for that
        // onVisibleChanged() will not be called when widget loaded
        Component.onCompleted: {
            if (root.visible) {
                updateValue() // re-get value is need here
                if (root.value) {
                    setValue(root.value)
                }
            }
        }
    }
    
    function saveKey() {
        print("ipv4 save key", section, key, value) // TODO test
        root.value = root.rightLoader.item.getValue()
        setKey()
    }
}
