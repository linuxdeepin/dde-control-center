import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import "../widgets/"
import DBus.Com.Deepin.Daemon.DateAndTime 1.0

Item {
    id: dateTimeModule
    anchors.fill: parent
    //color: bgColor 

    property string timeFont: "Maven Pro Light"
    property var gDate: DateAndTime {}

    property var date: new Date()
    property var weekNames: [dsTr("Sunday"), dsTr("Monday"), dsTr("Tuesday"), 
        dsTr("Wednesday"), dsTr("Thursday"), dsTr("Friday"), dsTr("Saturday"), 
        dsTr("Sunday")]

    Timer {
        running: true
        repeat: true
        interval: 500
        onTriggered: { parent.date = new Date() }
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

    Column {
        id: contentColumn
        anchors.top: parent.top
        anchors.topMargin: 48
        width: parent.width

        Rectangle {
            id: timeBox
            width: parent.width
            height: 118
            color: "#1a1b1b"

            DigitalTime {
                id: dynamicTime
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 24
            }

            Text {
                id: amPmText
                anchors.left: dynamicTime.right
                anchors.bottom: dynamicTime.bottom
                anchors.bottomMargin: -2
                color: "#666666"

                font.pixelSize: 14
                font.family: timeFont
                visible: !twentyFourHourSetBox.checked
                text: date.getHours() < 12 ? "AM" : "PM"
            }

            DLabel {
                id: dayText
                anchors.top: dynamicTime.bottom
                anchors.topMargin: 10
                anchors.horizontalCenter: dynamicTime.horizontalCenter

                font.pixelSize: 12
                text: date.getFullYear() + "-" + date.getMonth()+ "-"+ date.getDate() + ", " + weekNames[date.getDay()]
            }

        }

        DSepratorHorizontal {}

        DSwitcherButtonHeader {
            id: autoSetTimeBox
            text: dsTr("Auto-sync datetime")
            width: parent.width
            checked: gDate.autoSetTime

            onClicked: {
                gDate.SetAutoSetTime(checked)
            }
        }

        DSwitcherButtonHeader {
            id: twentyFourHourSetBox
            text: dsTr("24 Hour")
            checked: gDate.use24HourDisplay

            Timer{
                running: true
                repeat: true
                interval: 1000
                onTriggered: {
                    twentyFourHourSetBox.checked = gDate.use24HourDisplay
                }
            }

            onClicked: {
                gDate.use24HourDisplay = checked
            }
        }
    }

    Rectangle {
        id: timezoneTitle
        anchors.top: contentColumn.bottom
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
            width: parent.width - timezoneTitleText.width - 42
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: timezoneTitleText.right
            font.pixelSize: 13
            color: "#666666"
            elide: Text.ElideRight
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

        TimezoneData { id: timezoneData }

        ListView {
            anchors.fill: parent
            model: timezoneData.timezoneList
            delegate: TimezoneItem {}
            focus: true
            currentIndex: 19
            boundsBehavior: Flickable.DragOverBounds
            
            onCurrentItemChanged: {
                currentTimezone.text = currentItem.timezoneText
                gDate.SetTimeZone(timezoneData.getTimezoneByOffset(currentItem.timezoneValue))
                Date.timeZoneUpdated()
            }
        }
    }


    DSepratorHorizontal {
        anchors.top: timezoneList.bottom
    }

    DBaseHeader {
        id: dateBoxTitle
        anchors.top: timezoneList.bottom
        anchors.topMargin: 2
        width: parent.width
        height: 38

        title: DLabel {
            font.pixelSize: 13
            text: dsTr("Date")
        }

        button: DTextButton {
            text: dsTr("Change Date")
        }
    }

    Calendar {
        anchors.top: dateBoxTitle.bottom
    }
}
