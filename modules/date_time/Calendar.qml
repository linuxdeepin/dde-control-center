import QtQuick 2.1
import "calendar_core.js" as CalendarCore

Item {
    Rectangle {
        id: dateBoxAdjustment
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width
        height: 28
        color: "#1a1b1b"

        Image {
            id: decreaseYearButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 16

            source: "images/arrow_left.png"

            MouseArea {
                anchors.fill: parent
                onClicked: { yearAdjustment.yearNumber -= 1 }
            }
        }

        Text {
            id: yearAdjustment
            property int yearNumber: Number(date.getFullYear())
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: decreaseYearButton.right
            anchors.leftMargin: 12

            color: textColor
            font.pixelSize: 12
            text: yearNumber + "年"
        }

        Image {
            id: increaseYearButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: yearAdjustment.right
            anchors.leftMargin: 12

            source: "images/arrow_right.png"

            MouseArea {
                anchors.fill: parent
                onClicked: { yearAdjustment.yearNumber += 1 }
            }
        }

        Image {
            id: decreaseMonthButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 16

            source: "images/arrow_right.png"

            MouseArea {
                anchors.fill: parent
                onClicked: { 
                    if (monthAdjustment.monthNumber == 12) {

                        monthAdjustment.monthNumber = 1
                    }
                    else {
                        monthAdjustment.monthNumber += 1 
                    }
                }
            }
        }

        Text {
            id: monthAdjustment
            property int monthNumber: Number(date.getMonth() + 1)
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: decreaseMonthButton.left
            anchors.rightMargin: 12

            color: textColor
            font.pixelSize: 12
            text: monthNumber + "月"
        }

        Image {
            id: increaseMonthButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: monthAdjustment.left
            anchors.rightMargin: 12

            source: "images/arrow_left.png"

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (monthAdjustment.monthNumber == 1) {

                        monthAdjustment.monthNumber = 12
                    }
                    else {
                        monthAdjustment.monthNumber -= 1
                    }
                }
            }
        }
    }

    SepratorHorizontal {
        anchors.top: dateBoxAdjustment.bottom
    }

    CalendarRow {
        id: weekTitle
        anchors.top: dateBoxAdjustment.bottom
        anchors.topMargin: 1
    }

    GridView {
        anchors.top: weekTitle.bottom
        anchors.left: parent.left
        width: cellWidth * 7
        cellWidth: 44 
        cellHeight: 30
        model: ListModel {id: testModule}
        delegate: CalendarItem {}

        Component.onCompleted: {
            var dates = CalendarCore.getDates("2013-11-25");
            for (var i=0; i < dates.length; i++){
                testModule.append(dates[i])
            }
            height = (parseInt(dates.length/7) + 1) * cellHeight
        }
    }

    /***
    Column {
        anchors.top: weekTitle.bottom

        CalendarRow {
            sevenString: [
                {"text": "27", "grey": true},
                {"text": "28", "grey": true},
                {"text": "29", "grey": true},
                {"text": "30", "grey": true},
                {"text": "31", "grey": true},
                {"text": "1", "grey": false},
                {"text": "2", "grey": false},
            ]
        }

        CalendarRow {
            sevenString: [
                {"text": "3", "grey": false},
                {"text": "4", "grey": false},
                {"text": "5", "grey": false},
                {"text": "6", "grey": false},
                {"text": "7", "grey": false},
                {"text": "8", "grey": false},
                {"text": "9", "grey": false},
            ]
        }
    }
    ***/

}
