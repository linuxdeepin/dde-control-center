import QtQuick 2.1
import Deepin.Widgets 1.0
import "../widgets"

DBaseLine {
    id: root
    
    property string section
    property string key
    property string text
    
    visible: isKeyAvailable(key)
    
    color: dconstants.contentBgColor
    leftMargin: contentLeftMargin
    leftLoader.sourceComponent: DssH2{
        text: root.text
    }

    rightLoader.sourceComponent: Ipv4Input{
        width: valueWidth
        isError: isValueError(key)
        // TODO
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
                var value = getValue()
                setKey(root.section, root.key, value)
            }
        }
        
        // TODO
        // Component.onCompleted: {
        //     if(root.visible){
        //         var value = getKey(root.section, root.key)
        //         if(value){
        //             setValue(value)
        //         }
        //     }
        // }
    }
}
