import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

Row {
    width: childrenRect.width
    height: parent.height
    anchors.horizontalCenter: parent.horizontalCenter
    spacing: 10

    function init() {
        hoursEdit.text = globalDate.getHours() < 10 ? "0" + globalDate.getHours() : globalDate.getHours()
        minutesEdit.text = globalDate.getMinutes() < 10 ? "0" + globalDate.getMinutes() : globalDate.getMinutes()
        hoursEdit.forceActiveFocus()
    }

    signal accepted

    property string currentTime: hoursEdit.text + ":" + minutesEdit.text + ":0"

    Item {
        width: hoursEdit.width
        height: parent.height

        DArrowButton {
            visible: hoursEdit.activeFocus
            direction: "up"
            anchors.bottom: hoursEdit.top
            anchors.bottomMargin: 6
            anchors.horizontalCenter: hoursEdit.horizontalCenter

            onClicked: {
                hoursEdit.increase()
            }
        }

        TimeInput{
            id: hoursEdit
            anchors.verticalCenter: parent.verticalCenter
            min: 0
            max: 23
            activeFocusOnTab: true
            onToNext: {
                minutesEdit.forceActiveFocus()
            }

            onAccepted: parent.accepted()
        }

        DArrowButton {
            visible: hoursEdit.activeFocus
            direction: "down"
            anchors.top: hoursEdit.bottom
            anchors.topMargin: 6
            anchors.horizontalCenter: hoursEdit.horizontalCenter
            onClicked: {
                hoursEdit.decrease()
            }
        }
    }

    DLabel {
        id: secondColon
        font.family: fixedFont.name
        height: parent.height
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 55
        color: "white"
        text: ":"
    }

    Item {
        width: minutesEdit.width
        height: parent.height

        DArrowButton {
            visible: minutesEdit.activeFocus
            direction: "up"
            anchors.bottom: minutesEdit.top
            anchors.bottomMargin: 6
            anchors.horizontalCenter: minutesEdit.horizontalCenter
            onClicked: {
                minutesEdit.increase()
            }
        }

        TimeInput{
            id: minutesEdit
            anchors.verticalCenter: parent.verticalCenter
            min: 0
            max: 59
            activeFocusOnTab: true
            onToNext: {
                hoursEdit.forceActiveFocus()
            }
            onAccepted: parent.accepted()
        }

        DArrowButton {
            visible: minutesEdit.activeFocus
            direction: "down"
            anchors.top: minutesEdit.bottom
            anchors.topMargin: 6
            anchors.horizontalCenter: minutesEdit.horizontalCenter
            onClicked: {
                minutesEdit.decrease()
            }
        }
    }
}
