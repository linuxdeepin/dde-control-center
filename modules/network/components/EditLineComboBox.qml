import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseLine {
    id: root
    
    property string section
    property string key
    property string text
    
    color: dconstants.contentBgColor
    leftMargin: contentLeftMargin
    leftLoader.sourceComponent: DssH2{
        text: root.text
    }

    rightLoader.sourceComponent: DComboBox{
        id: securityCombobox    //TODO
        anchors.left: parent.left
        anchors.leftMargin: -3
        width: valueWidth
        text: getKey(section, key)

        property var menuLabels: connectionSessionObject.GetAvailableValues(section, key)

        function menuSelect(i){
            text = menuLabels[i]
            setKey(section, key, text)
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

    }
    
    Component.onCompleted: {
        // TODO test
        print(key, getKey(section, key), "[",connectionSessionObject.GetAvailableValues(section,key), "]")
    }
}
