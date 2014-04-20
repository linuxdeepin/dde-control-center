import QtQuick 2.1
import Deepin.Widgets 1.0
import "../widgets"

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: Ipv4Input{
        width: valueWidth
        isError: isValueError()
        // TODO fix focus issue
        // onToNext: {
            // var ipAddress = getValue()
            // if(ipAddress){
            //     var netmask = windowView.getDefaultMask(ipAddress)
            //     if(netmask){
            //         netmaskLine.rightLoader.item.setValue(netmask)
            //     }
            // }
            // netmaskLine.rightLoader.item.getFocus()
        // }
        onIsFocusChanged: {
            if(!isFocus){
                root.value = getValue()
                setKey()
            }
        }
        
        Component.onCompleted: {
            if(root.visible){
                value = getKey()
                if (value) {
                    setValue(value)
                }
            }
        }
    }
}
