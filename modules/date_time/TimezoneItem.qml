import QtQuick 2.1

Rectangle {
    anchors.left: parent.left
    width: parent.width
    height: 28
    color: "#1a1b1b"

    signal clicked

    property string timezoneValue: value
    property string timezoneText: textD

    Text {
        id: label
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: parent.verticalCenter

        font.pixelSize: 13
        color: parent.ListView.view.currentIndex == index ? Qt.rgba(0, 144/255, 1, 1.0) :dconstants.fgColor
        text: textD

        property var elideOpen: false

        Component.onCompleted: {
            if(width > parent.width - 25){
                label.width = parent.width - 25
                label.elide = Text.ElideRight
                label.elideOpen = true
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: label.elideOpen
        onClicked: {
            gDate.SetTimeZone(getTimeZoneInfoByOffset(parent.timezoneValue)[1])
        }

        onEntered: {
            toolTip.showTip(label.text)
        }
        onExited: {
            toolTip.hideTip()
        }
    }
}

