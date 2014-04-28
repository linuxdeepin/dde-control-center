import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseLine {
    id: root
    objectName: "BaseEditLine"
    
    property string key
    property string text
    property var value // mid-value between ConnectionSession and widget
    
    // update value even if other key changed
    property bool alwaysUpdate: false
    
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
                updateValue()
            }
        }
        // TODO test
        print("BaseEditLine.onVisibleChanged", visible ? "(show)" : "(hide)", section, key, value)
    }
    
    color: dconstants.contentBgColor
    property color normalColor: dconstants.contentBgColor
    property color errorColor: "#F48914"
    
    // TODO error state
    property bool showErrorConditon
    property bool showError: showErrorConditon && isValueError()
    Connections {
        target: rightLoader.item
        onActiveFocusChanged: {
            if (!rightLoader.item.activeFocus) {
                showErrorConditon = true
            }
        }
    }
    onShowErrorChanged: {
        border.color = showError ? errorColor : normalColor
        if (showError) {
            expandSection()
        }
    }
    
    leftMargin: contentLeftMargin
    leftLoader.sourceComponent: DssH2{
        text: root.text
    }
    
    function setKey() {
        print("BaseEditLine.setKey", section, key, value) // TODO test
        generalSetKey(section, key, value)
        updateKeysAlways()
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
    
    function updateValue() {
        value = getKey()
    }
    
    function isKeyAvailable() {
        return getIndexFromArray(key, availableKeys) != -1
    }
    
    function isValueError() {
        if (root.value == undefined) {
            return false
        }
        return errors[root.key] ? true : false
    }
    
    function getAvailableValues() {
        // TODO remove
        // return connectionSessionObject.GetAvailableValues(section, key)
        var valuesJSON = connectionSessionObject.GetAvailableValues(section, key);
        var values = unmarshalJSON(valuesJSON)
        return values
    }
    
    function getAvailableValuesText() {
        var values = getAvailableValues()
        var valuesText = []
        for (var i=0; i<values.length; i++) {
            valuesText.push(values[i].Text)
        }
        return valuesText
    }
    
    function getAvailableValuesTextByValue(value) {
        var values = getAvailableValues()
        if (values == null) {
            // TODO fixme, values is null here so this function should
            // not be called in this case
            print("==> [WARNING]", values, section, key, value) //TODO test
            return
        }
        for (var i=0; i<values.length; i++) {
            if (values[i].Value === value) {
                return values[i].Text
            }
        }
    }
    
    // overload this function if need
    function saveKey() {
        print("save key", section, key, value) // TODO test
        showErrorConditon = true
        setKey()
    }
}
