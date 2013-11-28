import QtQuick 2.1

Rectangle {
    id: dateTimeModule
    anchors.fill: parent
    color: childColor

    //property variant gDateTime: DateAndTime { }

    property color textColor: "#b4b4b4"

    property var date: new Date()
    property var _monthNames: [ "JAN", "FEB", "MAR", "APR", "MAY", "JUN","JUL", "AUG", "SEP", "OCT", "NOV", "DEC" ];

    Timer {
        running: true
        repeat: true
        interval: 500
        onTriggered: { parent.date = new Date() }
    }

    onDateChanged: {
        var hours = date.getHours()
        if (hours < 10) {
            hoursText.text = "0"+hours
        }
        else{
            hoursText.text = hours
        }

        if (hours < 12) {
            amPmText.text = "AM"
        }
        else {
            amPmText.text = "PM"
        }

        var minutes = date.getMinutes()
        if (minutes < 10){
            minutesText.text = "0"+minutes
        }
        else {
            minutesText.text = minutes
        }
        var weekNames = ["星期天", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"];

        dayText.text = date.getFullYear() + "年" + date.getMonth()+ "月"+ date.getDate() + "日" + ", " + weekNames[date.getDay()]
    }


    Text {
        id: datetimeTitle
        anchors.top: parent.top
        anchors.topMargin: 16
        anchors.left: parent.left
        anchors.leftMargin: 20
        font.pixelSize: 15
        font.bold: true
        color: textColor
        text: dsTr("Date & Time")
    }

    SepratorHorizontal {
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

        Rectangle {
            id: dynamicTime
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 24
            color: Qt.rgba(0, 0, 0, 0)

            width: 150
            height: 38

            Row {
                spacing: 10
                anchors.centerIn: parent

                Text {
                    id: hoursText
                    anchors.verticalCenter: parent.verticalCenter

                    font.pixelSize: 50
                    font.bold: true
                    color: "white"
                    text: date.getHours()
                }

                Text {
                    id: secondColon
                    anchors.verticalCenter: parent.verticalCenter

                    font.pixelSize: 50
                    font.bold: true
                    color: 'white'
                    text: ":"
                }

                Text {
                    id: minutesText
                    anchors.verticalCenter: parent.verticalCenter

                    font.pixelSize: 50
                    font.bold: true
                    color: "white"
                    text: date.getMinutes()
                }

                Text {
                    id: amPmText
                    anchors.bottom: minutesText.bottom
                    anchors.bottomMargin: 6
                    color: "#666666"

                    font.pixelSize: 20
                    font.bold: true
                }
            }
        }

        Text {
            id: dayText
            anchors.top: dynamicTime.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: dynamicTime.horizontalCenter

            color: "#666666"
            font.pixelSize: 12
        }

    }

    SepratorHorizontal {
        anchors.top: timeBox.bottom
    }

    SwitcherRect {
        id: autoSetTimeBox
        anchors.top: timeBox.bottom
        anchors.topMargin: 2
        text: dsTr("Auto-sync datetime")
        button.checked: true

        onClicked: {
            console.log("autoSetTimeBox: " + button.checked)
        }
    }

    SepratorHorizontal {
        anchors.top: autoSetTimeBox.bottom
    }

    SwitcherRect {
        id: twentyFourHourSetBox
        anchors.top: autoSetTimeBox.bottom
        anchors.topMargin: 2
        text: dsTr("24 Hour")
        onClicked: {
            console.log("twentyFourHourSetBox: "+ button.checked)
        }
    }

    SepratorHorizontal {
        anchors.top: twentyFourHourSetBox.bottom
    }

    Rectangle {
        id: timezoneTitle
        anchors.top: twentyFourHourSetBox.bottom
        anchors.topMargin: 2
        width: parent.width
        height: 28
        color: "transparent"

        Text {
            id: timezoneTitleText
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 18
            font.pixelSize: 13
            color: textColor
            text: dsTr("Timezone")
        }

        Text {
            id: currentTimezone
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: timezoneTitleText.right
            font.pixelSize: 13
            color: "#666666"
            text: "(UTC+08:00)北京时间"
        }

        Image {
            id: expandTimezoneSelect
            source: "images/arrow_down.png"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 8
        }
    }

    SepratorHorizontal {
        anchors.top: timezoneTitle.bottom
    }

    Rectangle {
        id: dateBoxTitle
        anchors.top: timezoneTitle.bottom
        anchors.topMargin: 2
        width: parent.width
        height: 28
        color: "transparent"

        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 18
            font.pixelSize: 13
            color: textColor
            text: dsTr("Date")
        }

        TextButton {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 8
            text: dsTr("Apply Changes")
        }
    }

    SepratorHorizontal {
        anchors.top: dateBoxTitle.bottom
    }

    Rectangle {
        id: calendarBox
        anchors.top: dateBoxTitle.bottom
        anchors.topMargin: 2
        anchors.left: parent.left
        width: parent.width

        Calendar {}
    }
}
