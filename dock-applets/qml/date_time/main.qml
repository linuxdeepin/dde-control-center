import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.DateAndTime 1.0
import DBus.Com.Deepin.Api.LunarCalendar 1.0
import "../widgets"
import "calendar_core.js" as CalendarCore

DockApplet{
    id: datetimeApplet
    title: "Date and Time"
    appid: "AppletDateTime"
    //icon: "digital" // analog
    icon: "vcalendar" // analog

    Component.onCompleted: setData("clockType", currentClockType)

    property string currentClockType: "digital"
    onCurrentClockTypeChanged: {
        setData("clockType", currentClockType)
    }

    property int xEdgePadding: 18
    property int rootWidth: 260

    property var locale: Qt.locale()
    property var globalDate: new Date()
    property var dbusDateAndTime: DateAndTime {
        onCurrentTimezoneChanged: {
            Date.timeZoneUpdated()
            globalDate = new Date()
        }
    }
    property var dbusLunarCalendar: LunarCalendar {}

    onActivate: {
        showDateTime(0)
    }

    property var typeNames: {
        "digital": dsTr("_Switch to analog"),
        "analog": dsTr("_Switch to digital")
    }

    function showDateTime(id){
        dbusControlCenter.ShowModule("date_time")
    }

    function hideDateTime(id){
        set_hide_applet("date_time")
    }

    function switchClockType(id){
        if(currentClockType == "digital"){
            currentClockType = "analog"
        }
        else{
            currentClockType = "digital"
        }
        var content_obj = unmarshalJSON(menu.content)
        content_obj.menuItems[id].itemText = typeNames[currentClockType]
        menu.content = marshalJSON(content_obj)
        print(id, currentClockType)
    }

    menu: Menu{
        Component.onCompleted: {
            addItem(dsTr("_Run"), showDateTime);
            addItem(typeNames[currentClockType], switchClockType);
        }
    }

    property int itemWidth: 36
    property int itemHeight: 36
    property int weekTitleHeight: 21

    window: DockQuickWindow {
        id: root
        width: rootWidth
        height: contentColumn.height
        color: "transparent"

        Column {
            id: contentColumn
            width: itemWidth * 7 - 1
            anchors.horizontalCenter: parent.horizontalCenter

            Item {
                id: topTitle
                width: parent.width
                height: 38

                DssH2 {
                    text: globalDate.toLocaleDateString(locale)
                    anchors.verticalCenter: parent.verticalCenter
                }

                DssH2 {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#fcdd51"
                    visible: false
                    text: {
                        var info = dbusLunarCalendar.GetLunarInfoBySolar(
                            globalDate.getFullYear(),
                            globalDate.getMonth() + 1,
                            globalDate.getDate()
                        )
                        return "农历" + info[0][3] + info[0][4]
                    }
                    Component.onCompleted: {
                        if(dssLocale.lang == "zh_CN"){
                            visible = true
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: Qt.rgba(1, 1, 1, 0.2)
            }


            GridView {
                id: weekTitleGridView
                width: itemWidth * 7
                height: weekTitleHeight
                cellWidth: itemWidth
                cellHeight: weekTitleHeight
                boundsBehavior: Flickable.StopAtBounds
                model: ListModel {id: weekTitleModel}
                delegate: WeekTitleItem{}
                currentIndex: globalDate.getDay()

                Component.onCompleted: {
                    weekTitleModel.append({"dayText": dsTr("Sun")})
                    weekTitleModel.append({"dayText": dsTr("Mon")})
                    weekTitleModel.append({"dayText": dsTr("Tue")})
                    weekTitleModel.append({"dayText": dsTr("Wed")})
                    weekTitleModel.append({"dayText": dsTr("Thu")})
                    weekTitleModel.append({"dayText": dsTr("Fri")})
                    weekTitleModel.append({"dayText": dsTr("Sat")})
                    currentIndex = globalDate.getDay()
                }
            }

            GridView {
                id: datesGridView
                width: itemWidth * 7
                height: itemHeight * 6
                cellWidth: itemWidth
                cellHeight: itemHeight
                model: ListModel { id: datesModel }
                delegate: DateItem {}
                boundsBehavior: Flickable.StopAtBounds
                focus: true

                function initDates(date_str) {
                    var dates = CalendarCore.getDates(date_str);
                    datesModel.clear()
                    for (var i=0; i < dates.length; i++){
                        datesModel.append(dates[i])
                        if (dates[i].dateValue == date_str) {
                            datesGridView.currentIndex = i
                        }
                    }
                }

                Component.onCompleted: {
                    var date_str = CalendarCore.dateToString(globalDate)
                    initDates(date_str)
                }

            }

        }
        
    }
}
