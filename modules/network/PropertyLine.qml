import QtQuick 2.1
import Deepin.Widgets 1.0

DCenterLine {
    id: propertyLine
    objectName: "PropertyLine"

    property var visibleType: ["all"]
    property string section: ""
    property string key: ""

    leftWidth: parent.leftWidth
    color: dconstants.contentBgColor
    visible: getIndexFromArray("all", visibleType) != -1 || getIndexFromArray(parent.selectVpnType, visibleType) != -1
    Component.onCompleted: {
        propertyLine.parent.updateLeftWidth(title.contentWidth)
    }
}
