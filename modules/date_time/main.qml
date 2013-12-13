import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import "../widgets/"
import DBus.Com.Deepin.Daemon.DateAndTime 1.0

Item {
    id: dateTimeModule
    anchors.fill: parent

    property string timeFont: "Maven Pro Light"
    property var gDate: DateAndTime {}
    property var dconstants: DConstants {}

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

    Column {
        id: contentColumn
        anchors.top: parent.top
        width: parent.width

        DssTitle { text: dsTr("Date & Time") }

        DSeparatorHorizontal {}

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

        DSeparatorHorizontal {}

        DSwitchButtonHeader {
            id: autoSetTimeBox
            text: dsTr("Auto-sync datetime")
            width: parent.width
            active: gDate.autoSetTime

            onClicked: {
                gDate.SetAutoSetTime(checked)
            }
        }

        DSeparatorHorizontal {}

        DSwitchButtonHeader {
            id: twentyFourHourSetBox
            text: dsTr("24 Hour")
            active: gDate.use24HourDisplay

            onClicked: {
                gDate.use24HourDisplay = active
            }
        }

        DSeparatorHorizontal {}

        DBaseExpand {
            id: timezoneExpand
            property string currentTimezoneLabel

            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Timezone")
                hintText: timezoneExpand.currentTimezoneLabel
                onClicked: {
                    timezoneExpand.expanded = !timezoneExpand.expanded
                }
            }
            content.sourceComponent: Rectangle {
                id: timezoneList
                width: parent.width
                height: 200
                clip: true
                color: dconstants.contentBgColor

                TimezoneData { id: timezoneData }

                DScrollWidget {
                    anchors.fill: parent
                    ListView {
                        id: timezone_listview
                        anchors.fill: parent
                        model: timezoneData.timezoneList
                        delegate: TimezoneItem {}
                        focus: true
                        currentIndex: 19
                        
                        onCurrentItemChanged: {
                            timezoneExpand.currentTimezoneLabel = currentItem.timezoneText
                            //gDate.SetTimeZone(timezoneData.getTimezoneByOffset(currentItem.timezoneValue))
                            //Date.timeZoneUpdated()
                        }
                    }
                }
            }
        }

        DBaseLine {
            id: dateBoxTitle
            width: parent.width
            height: 38

            leftLoader.sourceComponent: DLabel {
                font.pixelSize: 13
                text: dsTr("Date")
            }

            rightLoader.sourceComponent: DTextButton {
                text: dsTr("Change Date")
            }
        }

        DSeparatorHorizontal {}

        Calendar {}
    }
}
