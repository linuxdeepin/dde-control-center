import QtQuick 2.1
import QtQuick.Controls 1.0
import "../widgets/"

Item {
    id: sliderRect
    property alias value: dslider.value
    property string leftLabel: undefined
    property string rightLabel: undefined

    height: 20 + dslider.height
    width: 190

    Column {
        DSlider {
            id: dslider
            width: sliderRect.width - 4
            anchors.leftMargin: 2
            anchors.rightMargin: 2
        }

        Item {
            height: 20
            width: dslider.width

            DSliderLabelItem {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: leftLabel
            }

            DSliderLabelItem {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                text: rightLabel
            }
        }
    }

}
