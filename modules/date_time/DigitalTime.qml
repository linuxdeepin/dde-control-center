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

    Binding {
        target: hoursText
        property: "text"
        value: {
            if(globalDate.getHours() == 12 | globalDate.getHours() == 0){
                return 12
            }
            else{
                return use24Hour ? globalDate.getHours() : globalDate.getHours() % 12
            }
        }
        when: mouseAreaVisible
    }

    Binding {
        target: minutesText
        property: "text"
        value: globalDate.getMinutes() < 10 ? "0" + globalDate.getMinutes() : globalDate.getMinutes()
        when: mouseAreaVisible
    }

    Row {
        spacing: 10
        anchors.centerIn: parent

        DigitalInput {
            id: hoursText
            mouseAreaVisible: digitalTime.mouseAreaVisible
            IntValidator{bottom: 0; top: 23;}
            font.family: fixedFont.name
        }

        Text {
            id: secondColon
            anchors.verticalCenter: parent.verticalCenter

            font.pixelSize: 55
            font.family: fixedFont.name
            color: "white"
            text: ":"
        }

        DigitalInput {
            id: minutesText
            mouseAreaVisible: digitalTime.mouseAreaVisible
            IntValidator{bottom: 0; top: 59;}
            font.family: fixedFont.name

            onAccepted: {
            }
        }
    }
}
