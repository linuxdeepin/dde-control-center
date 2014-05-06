import QtQuick 2.1
import Deepin.Widgets 1.0

Rectangle {
    width: parent.width
    height: 28
    color: dconstants.contentBgColor

    property string title
    property string value
    property int fontSize: 12
    property int leftWidth: systemInfoModule.leftWidth
    property alias titleWidth: titleArea.contentWidth

    property bool valueWrapped: false

    Component.onCompleted: {
        if (dsslocale.lang == "en") {
            //fontSize = 11
            print(leftWidth)
        }
    }

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
        width: parent.width - leftWidth - 8
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
