import QtQuick 2.1

Rectangle {
    id: delegate

    width: delegate.GridView.view.cellWidth
    height: delegate.GridView.view.cellHeight
    color: Qt.rgba(0, 0, 0, 0)

    property int centerSpacing: 3
    property int verticalPadding: 3
    property int horizontalPadding: 10

    property color fontNormalColor: dconstants.fgColor
    property color fontHoverColor: dconstants.hoverColor
    property color fontPressedColor: dconstants.activeColor
    property color bgNormalColor: Qt.rgba(0, 0, 0, 0)
    property color bgPressedColor: Qt.rgba(0, 0, 0, 0.4)

    // set from model
    property string itemLabel: item_label
    property var itemValue: item_value
    property var itemZone: item_zone
    // set from model

    property var currentValue: "0x0" // set from delegate

    property bool selected: itemValue == currentValue
    property bool hovered: false

    signal selectAction(var itemValue,string timezone)

    Rectangle {
        id: contentBox
        anchors.centerIn: parent
        width: horizontalPadding * 2 + txt.implicitWidth
        height: verticalPadding * 2 + txt.height + centerSpacing
        color: delegate.selected ? bgPressedColor : bgNormalColor
        radius: 4

        Text {
            id: txt
            anchors.centerIn: parent
            color: {
                if(delegate.selected){
                    return fontPressedColor
                }
                else if(delegate.hovered){
                    return fontHoverColor
                }
                else{
                    return fontNormalColor
                }
            }
            text: itemLabel
            font.pixelSize: 12
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onEntered: {
            parent.hovered = true
        }

        onExited: {
            parent.hovered = false
            toolTip.hideTip()
        }

        onReleased: {
            parent.hovered = false
        }

        onClicked: {
            parent.selectAction(parent.itemValue,parent.itemZone)
        }
    }
}
