import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseLine {
    id: root
    
    property string key
    property string text
    property var value // mid-value between ConnectionSession and widget
    
    visible: false
    Binding on visible {
        value: isKeyAvailable()
    }
    onVisibleChanged: {
        if (visible) {
            if (value) {
                // reset key if value already defined
                setKey()
            } else {
                // get value only when undefined
                value = getKey()
            }
        }
        print("DBaseLine.onVisibleChanged", visible ? "(show)" : "(hide)", section, key, value)
    }
    
    color: dconstants.contentBgColor
    leftMargin: contentLeftMargin
    leftLoader.sourceComponent: DssH2{
        text: root.text
    }
    
    function setKey() {
        print("BaseEditLine.setKey", section, key, value) // TODO test
        generalSetKey(section, key, value)
    }

    function getKey() {
        // TODO remove
        // var value = generalGetKey(section, key)
        // if (value) {
        //     return value
        // } else {
        //     return "" //null
        // }
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
