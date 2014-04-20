import QtQuick 2.1
import Deepin.Widgets 1.0

DSwitchButtonHeader{
    property string section
    property string key
    
    color: dconstants.contentBgColor
    active: getKey(section, key)
    
    onActiveChanged: {
        // TODO test
        print("cheked:", active)
        setKey(section, active)
    }
}
