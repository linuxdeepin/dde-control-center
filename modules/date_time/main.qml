import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import "../widgets/"

Rectangle {
    id: dateTimeModule
    anchors.fill: parent
    color: bgColor 

    //property variant gDateTime: DateAndTime { }

    property string timeFont: "Maven Pro Light"

    property var date: new Date()

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
        color: fgColor
        text: dsTr("Date & Time")
    }

    DSepratorHorizontal {
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

                    font.pixelSize: 55
                    font.family: timeFont
                    color: "white"
                    text: date.getHours()
                }

                Text {
                    id: secondColon
                    anchors.verticalCenter: parent.verticalCenter

                    font.pixelSize: 55
                    font.family: timeFont
                    color: 'white'
                    text: ":"
                }

                Text {
                    id: minutesText
                    anchors.verticalCenter: parent.verticalCenter

                    font.pixelSize: 55
                    font.family: timeFont
                    color: "white"
                    text: date.getMinutes()
                }

            }
        }

        Text {
            id: amPmText
            anchors.left: dynamicTime.right
            anchors.bottom: dynamicTime.bottom
            anchors.bottomMargin: -2
            color: "#666666"

            font.pixelSize: 14
            font.family: timeFont
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

    DSepratorHorizontal {
        anchors.top: timeBox.bottom
    }

    DSwitcherRect {
        id: autoSetTimeBox
        anchors.top: timeBox.bottom
        anchors.topMargin: 2
        text: dsTr("Auto-sync datetime")
        button.checked: true

        onClicked: {
            console.log("autoSetTimeBox: " + button.checked)
        }
    }

    DSepratorHorizontal {
        anchors.top: autoSetTimeBox.bottom
    }

    DSwitcherRect {
        id: twentyFourHourSetBox
        anchors.top: autoSetTimeBox.bottom
        anchors.topMargin: 2
        text: dsTr("24 Hour")
        onClicked: {
            console.log("twentyFourHourSetBox: "+ button.checked)
        }
    }

    DSepratorHorizontal {
        anchors.top: twentyFourHourSetBox.bottom
    }

    Rectangle {
        id: timezoneTitle
        anchors.top: twentyFourHourSetBox.bottom
        anchors.topMargin: 2
        width: parent.width
        height: 28
        color: bgColor

        Text {
            id: timezoneTitleText
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 18
            font.pixelSize: 13
            color: fgColor
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

        ImageCheckButton {
            color: bgColor
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 12

            inactivatedNomralImage: "images/arrow_down_normal.png"
            inactivatedHoverImage: "images/arrow_down_hover.png"
            inactivatedPressImage: "images/arrow_down_press.png"

            activatedNomralImage: "images/arrow_up_normal.png"
            activatedHoverImage: "images/arrow_up_hover.png"
            activatedPressImage: "images/arrow_up_press.png"

            onActivateChanged: {
                if (activate){
                    timezoneList.height = 260
                }
                else {
                    timezoneList.height = 0
                }
            }
        }
    }

    Rectangle {
        id: timezoneList
        anchors.top: timezoneTitle.bottom
        anchors.left: parent.left
        width: parent.width
        height: 0
        clip: true
        color: "#1a1b1b"

        Behavior on height {
            NumberAnimation { 
                duration: 300 
            }
        }

        TimezoneData { id: timezoneDate }

        ListView {
            anchors.fill: parent
            model: timezoneDate.timezoneList
            delegate: TimezoneItem {}
            focus: true
            currentIndex: 19
            
            onCurrentItemChanged: {
                currentTimezone.text = currentItem.timezoneText
            }
        }
    }
    
    DSepratorHorizontal {
        anchors.top: timezoneList.bottom
    }

    Rectangle {
        id: dateBoxTitle
        anchors.top: timezoneList.bottom
        anchors.topMargin: 2
        width: parent.width
        height: 28
        color: bgColor

        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 18
            font.pixelSize: 13
            color: fgColor
            text: dsTr("Date")
        }

        TextButton {
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: -1
            anchors.right: parent.right
            anchors.rightMargin: 8
            text: dsTr("修改日期")
        }
    }

    DSepratorHorizontal {
        anchors.top: dateBoxTitle.bottom
    }

    Rectangle {
        id: calendarBox
        anchors.top: dateBoxTitle.bottom
        anchors.topMargin: 2
        anchors.left: parent.left
        width: parent.width

        Calendar { }
    }
}
