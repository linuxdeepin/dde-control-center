import QtQuick 2.1
import QtQuick.Controls 1.0
import "../widgets/"

Item {
    id: sliderRect
    property alias value: dslider.value
    property string leftLabel: ""
    property string rightLabel: ""

    height: 20 + dslider.height
    width: 180

    property int paddingRuler: 2

    Item {

        Item {
            height: 20
            width: dslider.width
            anchors.top: dslider.bottom
            anchors.topMargin: -4

            Rectangle {
                id: leftRuler
                width: 1
                height: 6
                anchors.left: parent.left
                anchors.leftMargin: 9
                color: "white"
                visible: leftLabel ? true : false
            }

            DssH3 {
                anchors.top: leftRuler.bottom
                anchors.topMargin: paddingRuler
                anchors.horizontalCenter: leftRuler.horizontalCenter
                text: leftLabel
            }

            Rectangle {
                id: rightRuler
                width: 1
                height: 6
                anchors.right: parent.right
                anchors.rightMargin: 8
                color: "white"
                visible: rightLabel ? true : false
            }

            DssH3 {
                anchors.top: rightRuler.bottom
                anchors.topMargin: paddingRuler
                anchors.horizontalCenter: rightRuler.horizontalCenter
                text: rightLabel
            }

        }

        DSlider {
            id: dslider
            width: sliderRect.width - 4
            anchors.leftMargin: 2
            anchors.rightMargin: 2
        }
    }

}
