import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseLine {
    id: editLine
    objectName: "BaseEditLine"

    property var connectionSession
    property var availableSections
    property var availableKeys
    property var errors
    property string section
    property string key
    property string text
    property var cacheValue // cache value between ConnectionSession and widget

    // if true, don't compare cache value with backend when setting key
    property bool setKeyAlways: false

    color: dconstants.contentBgColor

    signal widgetShown
    visible: false
    Binding on visible {
        value: isKeyAvailable()
    }
    Component.onCompleted: {
        if (visible) {
            // send widgetShown() signal is need here
            widgetShown()
        }
    }
    onVisibleChanged: {
        if (visible) {
            if (cacheValue === undefined) {
                // get cacheValue when it is undefined
                updateCacheValue()
            } else if (alwaysUpdate) {
                // get cacheValue if property "alwaysUpdate" is true
                updateCacheValue()
            } else {
                // reset key if cacheValue already defined
                setKey(cacheValue)
            }
            widgetShown()
        }
        print("-> BaseEditLine.onVisibleChanged", visible ? "(show)" : "(hide)", section, key, cacheValue) // TODO test
    }

    // update cacheValue even if other key changed
    property bool alwaysUpdate: false
    Connections {
        target: connectionSession
        onConnectionDataChanged: {
            if (visible && alwaysUpdate) {
                updateCacheValue()
            }
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
    onShowErrorChanged: {
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

    rightLoader.focus: true     // TODO fix active focus issue
    leftMargin: contentLeftMargin
    leftLoader.sourceComponent: DssH2{
        text: editLine.text
    }

    function setKey(v) {
        cacheValue = v
        if (cacheValue === getKey() && !isValueError() && !setKeyAlways) {
            return
        }
//        print("-> BaseEditLine.setKey()", section, key, cacheValue) // TODO test
        connectionSession.SetKey(section, key, marshalJSON(cacheValue))
    }

    function getKey() {
        var value = unmarshalJSON(connectionSession.GetKey(section, key))
//        print("-> BaseEditLine.getKey()", section, key, value) // TODO test
        return value
    }

    function updateCacheValue() {
        cacheValue = getKey()
        // print("-> updateCacheValue()", section, key, cacheValue) // TODO test
    }

    function isKeyAvailable() {
        var ok = getIndexFromArray(section, availableSections) != -1 && getIndexFromArray(key, availableKeys[section]) != -1
        return ok
    }

    function isValueError() {
        if (!errors) {
            return false
        }
        if (errors[section] && errors[section][key]) {
            return true
        }
        return false
    }

    function getAvailableValues() {
        var valuesJSON = connectionSession.GetAvailableValues(section, key);
        var values = unmarshalJSON(valuesJSON)
        return values
    }

    function getAvailableValuesValue() {
        var values = getAvailableValues()
        var valuesValue = []
        for (var i=0; i<values.length; i++) {
            valuesValue.push(values[i].Value)
        }
        return valuesValue
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
            // values is null here so this function should not be
            // called in this case
            print("-> [WARNING] getAvailableValuesTextByValue: values is null,", values, section, key, cacheValue) //TODO test
            return ""
        }
        for (var i=0; i<values.length; i++) {
            if (values[i].Value === cacheValue) {
                return values[i].Text
            }
        }
        print("-> [WARNING] getAvailableValuesTextByValue:", values, section, key, cacheValue) //TODO test
        return ""
    }

    function getAvailableValuesIndex() {
        var values = getAvailableValues()
        if (values == null) {
            return -1
        }
        for (var i=0; i<values.length; i++) {
            if (values[i].Value === cacheValue) {
                return i
            }
        }
        return -1
    }

    function checkKey() {
        print("-> check key", section, key, cacheValue) // TODO test
        showErrorConditon = true
    }
}
