import QtQuick 2.1

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
            property int monthNumber: Number(date.getMonth())
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
        anchors.topMargin: 2
    }

    Column {
        anchors.top: weekTitle.bottom
    }

}
