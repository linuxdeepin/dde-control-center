import QtQuick 2.1
import "calendar_core.js" as CalendarCore

Rectangle {
    id: calendarComponent
    width: calendarItemCellWidth * 7
    height: calendarItemCellHeight * 6 + weekTitle.height
    color: "#1a1b1b"

    property alias weekTitleModel: weekTitleModel
    property alias datesGridView: datesGridView
    property alias datesModule: datesModule

    property int calendarItemCellWidth: 44
    property int calendarItemCellHeight: 38

    property var clickedDateObject: new Date()
    property var currentDateString: CalendarCore.dateToString(globalDate)

    property var debug_dates
    
    function updateDates(date_str) {
        var dates = CalendarCore.getDates(date_str);

        datesModule.clear()
        for (var i=0; i < dates.length; i++){
            datesModule.append(dates[i])
            if (dates[i].dateValue == date_str) {
                datesGridView.currentIndex = i
            }
        }
        datesGridView.height = (parseInt(dates.length/7) + 1) * datesGridView.cellHeight
        debug_dates = dates
    }

    ListView {
        id: weekTitle
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: 30
        delegate: CalendarTitleItem {}
        model: ListModel {id: weekTitleModel}
        orientation: ListView.Horizontal

        Component.onCompleted: {
            weekTitleModel.append({"dayText": dsTr("日"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("一"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("二"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("三"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("四"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("五"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("六"), "isHighlight": false})
        }
    }

    GridView {
        id: datesGridView
        anchors.top: weekTitle.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: cellWidth * 7
        cellWidth: calendarItemCellWidth
        cellHeight: calendarItemCellHeight
        model: ListModel {id: datesModule}
        delegate: CalendarItem {
            width: calendarItemCellWidth
            height: calendarItemCellHeight
            currentDateString: calendarComponent.currentDateString
        }
        focus: true
        currentIndex: -1

        Component.onCompleted: { 
            updateDates(CalendarCore.dateToString(clickedDateObject))
        }

        function getDelegateInstanceAt(index) {
            var item = contentItem.children[index];
            return item;
        }
    }

    Timer {
        id: dateTick
        running: false
        repeat: true
        interval: 500
        onTriggered: {
            updateDates(CalendarCore.dateToString(clickedDateObject))
        }
    }
}
