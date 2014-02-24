import QtQuick 2.1
import "calendar_core.js" as CalendarCore

Rectangle {
    id: calendarComponent
    width: calendarItemCellWidth * 7
    height: calendarItemCellHeight * 6 + weekTitle.height
    color: "#1a1b1b"

    property alias weekTitleModel: weekTitleModel
    property alias datesGridView: datesGridView
    property alias datesModel: datesModel

    property int calendarItemCellWidth: 44
    property int calendarItemCellHeight: 38

    property var clickedDateObject: new Date()
    property var currentDateString: CalendarCore.dateToString(globalDate)

    onClickedDateObjectChanged: {
        updateSelectedIndex(CalendarCore.dateToString(clickedDateObject))
    }

    function updateSelectedIndex(date_str){
        for(var i=0; i < datesModel.count; i++){
            if(datesModel.get(i)["dateValue"] == date_str){
                datesGridView.currentIndex = i
                return
            }
        }
    }
    
    function initDates(date_str) {
        var dates = CalendarCore.getDates(date_str);
        datesModel.clear()
        for (var i=0; i < dates.length; i++){
            datesModel.append(dates[i])
            if (dates[i].dateValue == date_str) {
                datesGridView.currentIndex = i
            }
        }
        //datesGridView.height = (parseInt(dates.length/7) + 1) * datesGridView.cellHeight
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
            weekTitleModel.append({"dayText": dsTr("Sun"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("Mon"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("Tue"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("Wed"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("Thu"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("Fri"), "isHighlight": false})
            weekTitleModel.append({"dayText": dsTr("Sat"), "isHighlight": false})
        }
    }

    GridView {
        id: datesGridView
        anchors.top: weekTitle.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: cellWidth * 7
        height: childrenRect.height
        cellWidth: calendarItemCellWidth
        cellHeight: calendarItemCellHeight
        model: ListModel {id: datesModel}
        delegate: CalendarItem {
            width: calendarItemCellWidth
            height: calendarItemCellHeight
            currentDateString: calendarComponent.currentDateString
        }
        focus: true
        currentIndex: -1

        Component.onCompleted: { 
            initDates(CalendarCore.dateToString(clickedDateObject))
        }

        function getDelegateInstanceAt(index) {
            var item = contentItem.children[index];
            return item;
        }
    }
}
