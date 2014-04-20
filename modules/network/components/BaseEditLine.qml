import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseLine {
    id: root
    
    property string key
    property string text
    property var value
    
    visible: isKeyAvailable()
    onVisibleChanged: {
        print("DBaseLine.onVisibleChanged", visible ? "(show)" : "(hide)", section, key, value)
        // TODO
        if (visible) {
            if (value == undefined) {
                // get value only when undefined
                value = getKey()
            } else {
                // reset key if value defined
                setKey()
            }
        }
    }
    
    color: dconstants.contentBgColor
    leftMargin: contentLeftMargin
    leftLoader.sourceComponent: DssH2{
        text: root.text
    }
    
    Component.onCompleted: {
        if (visible) {
            value = getKey()
            print("DBaseLine.onCompleted(show)", section, key, value)
        }
    }
    
    function setKey() {
        print("BaseEditLine.setKey", section, key, value) // TODO test
        generalSetKey(section, key, value)
    }

    function getKey() {
        return generalGetKey(section, key)
    }
    
    function isKeyAvailable() {
        return getIndexFromArray(key, availableKeys) != -1
    }
    
    function isValueError() {
        return errors[root.key] ? true : false
    }
    
    function getAvailableValues() {
        return connectionSessionObject.GetAvailableValues(section, key)
    }
}
