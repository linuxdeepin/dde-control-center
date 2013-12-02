import QtQuick 2.1

Rectangle {
    width: parent.width
    height: 28
    color: "#1a1b1b"

    property string title
    property string value
    property int fontSize: lang == "en_US" ? 11 : 12
    property int leftWidth: lang == "en_US" ? 100 : 80

    property bool valueWrapped: false

    Rectangle {
        id: itemTitleBox
        anchors.top: parent.top
        anchors.left: parent.left
        width: leftWidth
        height: parent.height
        color: "transparent"

        Text {
            anchors.right: parent.right
            anchors.rightMargin: 5
            //anchors.verticalCenter: parent.verticalCenter
            anchors.top: parent.top
            color: fgColor
            font.pixelSize: fontSize
            text: title
        }
    }

    Text {
        id: systemVersion
        anchors.left: itemTitleBox.right
        anchors.leftMargin: 5
        //anchors.verticalCenter: itemTitleBox.verticalCenter
        anchors.top: parent.top
        anchors.topMargin: 1
        color: fgColor
        font.pixelSize: fontSize
        text: value
        width: parent.width - leftWidth
        //elide: Text.ElideRight
        lineHeight: 1.4

        Component.onCompleted: {
            if (width < contentWidth){
                valueWrapped = true
                wrapMode = Text.WordWrap
                parent.height += 14
            }
        }
    }
}
