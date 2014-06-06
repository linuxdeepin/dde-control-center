import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseLine {
    id: editLine
    objectName: "BaseEditLine"
    
    property var connectionSession
    property var availableSections
    property var availableKeys
    property var connectionData
    property var errors
    property string section
    property string key
    property string text
    property var cacheValue // cache value between ConnectionSession and widget
    
    signal widgetShown
    visible: false
    Binding on visible {
        value: isKeyAvailable()
    }
    onVisibleChanged: {
        if (visible) {
            if (cacheValue !== undefined) {
                // reset key if cacheValue already defined
                setKey()
            } else {
                // get cacheValue only when undefined
                updateCacheValue()
            }
            widgetShown()
        }
        print("-> BaseEditLine.onVisibleChanged", visible ? "(show)" : "(hide)", section, key, cacheValue) // TODO test
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
    
    // update cacheValue even if other key changed
    property bool alwaysUpdate: false
    onConnectionDataChanged: {
        if (visible && alwaysUpdate) {
            updateCacheValue()
        }
    }
    
    // error state
    property bool showErrorConditon: false // will be true when widget focus changed or save button pressed
    property bool showError: showErrorConditon && isValueError()
    Connections {
        target: rightLoader.item
        onActiveFocusChanged: {
            print("-> onActiveFocusChanged", section, key, rightLoader.item.activeFocus) // TODO test
            if (!rightLoader.item.activeFocus) {
                showErrorConditon = true
            }
        }
    }
    onErrorsChanged: {
        if (isValueError()) {
            // print("-> [error] %1[%2]: %3".arg(section).arg(key).arg(errors[section][key])) // TODO test
        }
    }
    onShowErrorChanged: {
        // TODO border color or text color
        // border.color = showError ? errorColor : normalBorderColor
        leftLoader.item.color = showError ? errorColor : normalColor
        if (showError) {
            for (var p = parent;; p = p.parent) {
                if (p) {
                    if (p.objectName == "BaseEditSection") {
                        p.expandSection()
                        break
                    }
                } else {
                    break
                }
            }
        }
    }
    
    leftMargin: contentLeftMargin
    leftLoader.sourceComponent: DssH2{
        text: editLine.text
    }
    
    function setKey() {
        // print("-> BaseEditLine.setKey()", section, key, cacheValue) // TODO test
        connectionSession.SetKey(section, key, marshalJSON(cacheValue))
    }

    function getKey() {
        return unmarshalJSON(connectionSession.GetKey(section, key))
    }
    
    function updateCacheValue() {
        cacheValue = getKey()
        // print("-> updateCacheValue()", section, key, cacheValue) // TODO test
    }
    
    function isKeyAvailable() {
        return getIndexFromArray(section, availableSections) != -1 && getIndexFromArray(key, availableKeys[section]) != -1
    }
    
    function isValueError() {
        if (!visible || cacheValue == undefined) {
            return false
        }
        return errors[section][key] ? true : false
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
            print("-> [WARNING] getAvailableValuesTextByValue", values, section, key, cacheValue) //TODO test
            return
        }
        for (var i=0; i<values.length; i++) {
            if (values[i].Value === cacheValue) {
                return values[i].Text
            }
        }
        print("-> [WARNING] getAvailableValuesTextByValue():", values, section, key, cacheValue) //TODO test
        return ""
    }
    
    function getAvailableValuesIndex() {
        var values = getAvailableValues()
        if (values == null) {
            return 0
        }
        for (var i=0; i<values.length; i++) {
            if (values[i].Value === cacheValue) {
                return i
            }
        }
        print("-> [WARNING] getAvailableValuesIndex():", values, section, key, cacheValue) //TODO test
        return 0
    }
    
    function checkKey() {
        print("-> check key", section, key, cacheValue) // TODO test
        showErrorConditon = true
    }
}
