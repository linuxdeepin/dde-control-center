import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseLine {
    id: root
    
    property string section
    property string key
    property string text
    property int echoMode
    
    color: dconstants.contentBgColor
    leftMargin: contentLeftMargin
    leftLoader.sourceComponent: DssH2{
        text: root.text
    }

    rightLoader.sourceComponent: DTextInput{
        id: passwordArea
        width: valueWidth
        echoMode: root.echoMode
        // echoMode: TextInput.Password
        text: getKey(section, key)
        onActiveFocusChanged: {
            if(!activeFocus){
                print("focus changed psk:", text) //TODO test
                save()
            }
        }
        function save(){
            print("doSetKey psk:", text) //TODO test
            setKey(securitySettings.sectionName, "psk", text)
        }
    }
    
}

