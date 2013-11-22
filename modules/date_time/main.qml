import QtQuick 2.1
import Deepin.Daemon.gdatetime 1.0

Rectangle {
    anchors.fill: parent
    color: childColor

    property variant gDateTime: DateTimeMechanism { path: "/" }

    property color textColor: "#b4b4b4"

    Text {
        id: datetimeTitle
        anchors.top: parent.top
        anchors.topMargin: 16
        anchors.left: parent.left
        anchors.leftMargin: 20
        font.pixelSize: 15
        font.bold: true
        color: textColor
        text: "时间和日期"
    }

    SepratorLines {
        anchors.top: parent.top
        anchors.topMargin: 46
    }

    Rectangle {
        id: timeBox
        anchors.top: parent.top
        anchors.topMargin: 48
        width: parent.width
        height: 118
        color: "#1a1b1b"
    }

    SepratorLines {
        anchors.top: timeBox.bottom
    }

    SwitcherRect {
        id: autoSetTimeBox
        anchors.top: timeBox.bottom
        anchors.topMargin: 2
        text: "自动设置时间"
        button.checked: true

        onClicked: {
            console.log("autoSetTimeBox: " + button.checked)
        }
    }
    
    SepratorLines {
        anchors.top: autoSetTimeBox.bottom
    }

    SwitcherRect {
        id: twentyFourHourSetBox
        anchors.top: autoSetTimeBox.bottom
        anchors.topMargin: 2
        text: "24小时制"
        onClicked: {
            console.log("twentyFourHourSetBox: "+ button.checked)
        }
    }

    SepratorLines {
        anchors.top: twentyFourHourSetBox.bottom
    }

    Rectangle {
        id: timezoneTitle
        anchors.top: twentyFourHourSetBox.bottom
        anchors.topMargin: 2
        width: parent.width
        height: 28
        color: "#1a1b1b"

        Text {
            id: timezoneTitleText
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 18
            font.pixelSize: 13
            color: textColor
            text: "时区"
        }

        Text {
            id: currentTimezone
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: timezoneTitleText.right
            font.pixelSize: 13
            color: "#393a3a"
            text: "(UTC+08:00)北京时间"
        }
    }

    SepratorLines {
        anchors.top: timezoneTitle.bottom
    }

}
