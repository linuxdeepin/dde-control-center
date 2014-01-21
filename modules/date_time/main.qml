import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.DateAndTime 1.0
import "calendar_core.js" as CalendarCore

Item {
    id: dateTimeModule
    anchors.fill: parent

    property string timeFont: "Maven Pro Light"
    property var gDate: DateAndTime {
        onCurrentTimeZoneChanged: {
            Date.timeZoneUpdated()
            globalDate = new Date()
        }
    }
    property var dconstants: DConstants {}
    property string lang: 'en'
    property var locale: Qt.locale()

    property var globalDate: new Date()
    
    Component.onCompleted: {
        lang = dsslocale.lang
    }

    Timer {
        running: true
        repeat: true
        interval: 500
        onTriggered: { 
            parent.globalDate= new Date()
            lang = dsslocale.lang
            //dynamicTime.secondColonDisplay = !dynamicTime.secondColonDisplay
        }
    }

    Column {
        id: beforeTimeZoneArea
        anchors.top: parent.top
        width: parent.width
        height: childrenRect.height

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
                use24Hour: twentyFourHourSetBox.active
            }

            Text {
                id: amPmText
                anchors.left: dynamicTime.right
                anchors.bottom: dynamicTime.bottom
                anchors.bottomMargin: -2
                color: "#666666"

                font.pixelSize: 14
                font.family: timeFont
                visible: !twentyFourHourSetBox.active
                text: globalDate.getHours() < 12 ? "AM" : "PM"
            }

            DLabel {
                id: dayText
                anchors.top: dynamicTime.bottom
                anchors.topMargin: 10
                anchors.horizontalCenter: dynamicTime.horizontalCenter

                font.pixelSize: 12
                text: globalDate.toLocaleDateString(locale)
            }

        }

        DSeparatorHorizontal {}

        DSwitchButtonHeader {
            id: autoSetTimeBox
            text: dsTr("Auto-sync datetime")
            width: parent.width
            active: gDate.autoSetTime

            onClicked: {
                gDate.SetAutoSetTime(active)
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
    }

    TimezoneExpand {
        id: timeZoneArea
        anchors.top: beforeTimeZoneArea.bottom
        property int leftHeight: parent.height - beforeTimeZoneArea.height - afterTimeZoneArea.height - 40
    }

    Column {
        id: afterTimeZoneArea
        anchors.top: timeZoneArea.bottom

        DSeparatorHorizontal {}

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
                onClicked: {
                    print(calendarObj.cur_calendar.clickedDateObject)
                }
            }
        }

        DSeparatorHorizontal {}

        Calendar { id: calendarObj }
    }
}
