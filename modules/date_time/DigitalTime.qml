import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import "../widgets/"

Item {
    width: 150
    height: 38

    property bool inEdit: false
    property int digitalSize: 55

    Row {
        spacing: 10
        anchors.centerIn: parent


        TextInput {
            id: hoursText
            anchors.verticalCenter: parent.verticalCenter

            font.pixelSize: digitalSize
            font.family: timeFont
            text: twentyFourHourSetBox.checked ? date.getHours() : date.getHours() % 12
            color: "white"

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.IBeamCursor
            }
        }

        Text {
            id: secondColon
            anchors.verticalCenter: parent.verticalCenter

            font.pixelSize: digitalSize
            font.family: timeFont
            color: 'white'
            text: ":"
        }

        Text {
            id: minutesText
            anchors.verticalCenter: parent.verticalCenter

            font.pixelSize: digitalSize
            font.family: timeFont
            color: "white"
            text: date.getMinutes() < 10 ? "0" + date.getMinutes() : date.getMinutes()
        }
    }
}
