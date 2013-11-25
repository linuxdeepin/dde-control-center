import QtQuick 2.1

Rectangle {
    id: calendarRow
    anchors.horizontalCenter: parent.horizontalCenter
    width: 310
    height: 30
    color: "#1a1b1b"
    property variant sevenString: [
        {"text": "日", "grey": false},
        {"text": "一", "grey": false},
        {"text": "二", "grey": false},
        {"text": "三", "grey": false},
        {"text": "四", "grey": false},
        {"text": "五", "grey": false},
        {"text": "六", "grey": false},
    ]

    Row {
        CalendarItem { 
            inText.text: calendarRow.sevenString[0]['text']
            grey: calendarRow.sevenString[0]['grey']
        }

        CalendarItem { 
            inText.text: calendarRow.sevenString[1]['text']
            grey: calendarRow.sevenString[1]['grey']
        }

        CalendarItem { 
            inText.text: calendarRow.sevenString[2]['text']
            grey: calendarRow.sevenString[2]['grey']
        }

        CalendarItem { 
            inText.text: calendarRow.sevenString[3]['text']
            grey: calendarRow.sevenString[3]['grey']
        }

        CalendarItem { 
            inText.text: calendarRow.sevenString[4]['text']
            grey: calendarRow.sevenString[4]['grey']
        }

        CalendarItem { 
            inText.text: calendarRow.sevenString[5]['text']
            grey: calendarRow.sevenString[5]['grey']
        }

        CalendarItem { 
            inText.text: calendarRow.sevenString[6]['text']
            grey: calendarRow.sevenString[6]['grey']
        }
    }
}
