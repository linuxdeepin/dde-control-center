import QtQuick 2.1
import "../widgets"

Rectangle {
    width: parent.width
    height: 28
    color: dconstants.contentBgColor

    property string title
    property string value
    property int fontSize: 11
    property int leftWidth: 100

    property bool valueWrapped: false


     DLabel {
        id: titleArea
        width: leftWidth
        anchors.verticalCenter: parent.verticalCenter
        horizontalAlignment: Text.AlignRight
        font.pixelSize: fontSize
        text: title + " : "
    }

    DLabel {
        id: systemVersion
        anchors.left: titleArea.right
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: fontSize
        width: parent.width - leftWidth
        text: value

        Component.onCompleted: {
            if (width < contentWidth){
                valueWrapped = true
                wrapMode = Text.WordWrap
                parent.height += 14
            }
        }
    }
}
