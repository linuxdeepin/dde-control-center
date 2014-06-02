import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseLine {
    id: editLine
    objectName: "BaseEditLine"
    
    // TODO
    // property var connectionSession
    property var connectionData: connectionSession.data
    property string section
    property string key
    property string text
    property var value // mid-value between ConnectionSession and widget
    
    // update value even if other key changed
    property bool alwaysUpdate: false
    onConnectionDataChanged: {
        function updateKeysAlways() {
            if (visible && alwaysUpdate) {
                updateValue()
            }
        }
    }
    
    // TODO remove
    // Connections: {
    //     target: connectionSession
    //     onDataChanged: {
    //         function updateKeysAlways() {
    //             if (visible && alwaysUpdate) {
    //                 updateValue()
    //             }
    //         }
    //     }
    // }
    
    property var errors: connectionSession.errors[section] // TODO: what if missing error section
    onErrorsChanged: {
        if (isValueError()) {
            print("-> [error] %1[%2]: %3".arg(section).arg(key).arg(errors[key]))
        }
    }
    
    signal widgetShown
    visible: false
    Binding on visible {
        value: isKeyAvailable()
    }
    onVisibleChanged: {
        if (visible) {
            if (value !== undefined) {
                // reset key if value already defined
                setKey()
            } else {
                // get value only when undefined
                updateValue()
            }
            widgetShown()
        }
        // TODO test
        print("-> BaseEditLine.onVisibleChanged", visible ? "(show)" : "(hide)", section, key, value)
    }
    Component.onCompleted: {
        if (visible) {
            // send widgetShown() signal is need here
            widgetShown()
        }
    }
    
    // colors
    color: dconstants.contentBgColor
    property color normalBorderColor: dconstants.contentBgColor
    property color normalColor: dconstants.fgColor
    property color errorColor: "#F48914"
    
    // error state
    property bool showErrorConditon: false // will be true when widget focus changed or save button pressed
    property bool showError: showErrorConditon && isValueError()
    Connections {
        target: rightLoader.item
        onActiveFocusChanged: {
            print("-> onActiveFocusChanged", rightLoader.item.activeFocus) // TODO test
            if (!rightLoader.item.activeFocus) {
                showErrorConditon = true
            }
        }
    }
    onShowErrorChanged: {
        // TODO border color or text color
        // border.color = showError ? errorColor : normalBorderColor
        leftLoader.item.color = showError ? errorColor : normalColor
        if (showError) {
            if (parent.objectName == "BaseEditSection") {
                parent.expandSection()
            }
        }
    }
    
    leftMargin: contentLeftMargin
    leftLoader.sourceComponent: DssH2{
        text: editLine.text
    }
    
    function setKey() {
        print("-> BaseEditLine.setKey()", section, key, value) // TODO test
        connectionSession.SetKey(section, key, marshalJSON(value))
    }

    function getKey() {
        return unmarshalJSON(connectionSession.GetKey(section, key))
    }
    
    function updateValue() {
        value = getKey()
        print("-> updateValue()", section, key, value) // TODO test
    }
    
    function isKeyAvailable() {
        var availableSections = connectionSession.availableSections
        var availableKeys = connectionSession.availableKeys[section]
        return getIndexFromArray(section, availableSections) != -1 && getIndexFromArray(key, availableKeys) != -1
    }
    
    function isValueError() {
        if (editLine.value == undefined) {
            return false
        }
        return errors[editLine.key] ? true : false
    }
    
    function getAvailableValues() {
        var valuesJSON = connectionSession.GetAvailableValues(section, key);
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
    
    function getAvailableValuesTextByValue() {
        var values = getAvailableValues()
        if (values == null) {
            // values is null here so this function should
            // not be called in this case
            print("-> [WARNING] getAvailableValuesTextByValue", values, section, key, value) //TODO test
            return
        }
        for (var i=0; i<values.length; i++) {
            if (values[i].Value === value) {
                return values[i].Text
            }
        }
        print("-> [WARNING] getAvailableValuesTextByValue():", values, section, key, value) //TODO test
        return ""
    }
    
    function getAvailableValuesIndex() {
        var values = getAvailableValues()
        if (values == null) {
            return 0
        }
        for (var i=0; i<values.length; i++) {
            if (values[i].Value === value) {
                return i
            }
        }
        print("-> [WARNING] getAvailableValuesIndex():", values, section, key, value) //TODO test
        return 0
    }
    
    function checkKey() {
        print("-> check key", section, key, value) // TODO test
        showErrorConditon = true
    }
}
