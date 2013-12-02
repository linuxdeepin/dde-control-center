import QtQuick 2.1
import "calendar_core.js" as CalendarCore

Rectangle {
    width: 308
    //height: weekTitle.height + datesGridView.height
    color: "#1a1b1b"

    property alias weekTitleModel: weekTitleModel
    property alias datesGridView: datesGridView
    property alias datesModule: datesModule

    property var clickedDateObject: new Date()

    property var debug_dates
    
    function updateDates(date_str) {
        var currentDateString = CalendarCore.dateToString(new Date())
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
            weekTitleModel.append({"dayText": "日", "isHighlight": false})
            weekTitleModel.append({"dayText": "一", "isHighlight": false})
            weekTitleModel.append({"dayText": "二", "isHighlight": false})
            weekTitleModel.append({"dayText": "三", "isHighlight": false})
            weekTitleModel.append({"dayText": "四", "isHighlight": false})
            weekTitleModel.append({"dayText": "五", "isHighlight": false})
            weekTitleModel.append({"dayText": "六", "isHighlight": false})
        }
    }

    GridView {
        id: datesGridView
        anchors.top: weekTitle.bottom
        //anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        width: cellWidth * 7
        cellWidth: 44 
        cellHeight: 30
        model: ListModel {id: datesModule}
        delegate: CalendarItem {}
        focus: true
        currentIndex: -1

        Component.onCompleted: { 
            updateDates(CalendarCore.dateToString(clickedDateObject))
            //dateTick.start()
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
