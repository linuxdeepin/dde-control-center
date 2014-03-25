import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

Item {
    id: digitalTime
    width: 150
    height: 38

    property bool use24Hour: false
    property bool mouseAreaVisible: true
    signal doubleClicked()

    property int fontSize: 55
    property color fontColor: "white"

    Row {
        spacing: 4
        anchors.verticalCenter: parent.verticalCenter
        width: childrenRect.width
        height: childrenRect.height
        x: (parent.width - width)/2

        Behavior on x{
            PropertyAnimation { duration: 200 }
        }

        DLabel {
            id: hoursText
            font.family: fixedFont.name
            font.pixelSize: fontSize
            color: fontColor
            text: {
                var currentHour = globalDate.getHours()
                if(use24Hour){
                    return currentHour
                }
                else{
                    if(currentHour == 12 | currentHour == 0){
                        return 12
                    }
                    else{
                        return currentHour % 12
                    }
                }
            }
        }

        DLabel {
            id: secondColon
            font.family: fixedFont.name
            font.pixelSize: fontSize
            color: fontColor
            text: ":"
        }

        DLabel {
            id: minutesText
            font.family: fixedFont.name
            font.pixelSize: fontSize
            color: fontColor
            text: globalDate.getMinutes() < 10 ? "0" + globalDate.getMinutes() : globalDate.getMinutes()
        }
    }

    MouseArea {
        anchors.fill: parent
        onDoubleClicked: {
            parent.doubleClicked()
        }
    }
}
