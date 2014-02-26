import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.InputDevices 1.0

Item {
    id: keyboardModule
    anchors.fill: parent

    property int contentLeftMargin: 22
    property int contentHeight: 60
    property int sliderWidth: 180

    property var touchPadID: TouchPad {}

    Column {
        anchors.top: parent.top
        width: parent.width

        DBaseLine {
            leftLoader.sourceComponent: DssH2 {
                color: "white"
                font.bold: true
                text: dsTr("TouchPad")
            }
        }

        DSeparatorHorizontal {}

        DCenterLine {
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Enabled")
            content.sourceComponent: Item {
                width: 180
                height: parent.height
                DSwitchButton {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    checked: touchPadID.tPadEnable ? true : false
                    onClicked: {
                        touchPadID.tPadEnable = checked
                    }
                }
            }
        }

        DCenterLine {
            height: 38
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Button Order")
            content.sourceComponent: DRadioButton {
                width: 180
                anchors.verticalCenter: parent.verticalCenter
                initializeIndex: touchPadID.useHabit ? 1 : 0
                buttonModel: [
                    {"buttonId": "right_hand", "buttonLabel": dsTr("Right Hand")},
                    {"buttonId": "left_hand", "buttonLabel": dsTr("Left Hand")}
                ]

                onItemSelected: {
                    touchPadID.useHabit = idx == 1 ? true : false
                }
            }
        }

        DCenterLine {
            height: contentHeight
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Pointer Move Speed")
            content.sourceComponent: DSliderRect {
                leftLabel: dsTr("Slow")
                rightLabel: dsTr("Fast")

                value: (touchPadID.moveSpeed - 1)/9
                realValue: touchPadID.moveSpeed
                onValueChanged: {
                    if (touchPadID.moveSpeed!= (9 * value + 1)){
                        touchPadID.moveSpeed = 9 * value + 1
                    }
                }
            }
        }

        DCenterLine {
            height: contentHeight
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Pointer Accuracy")
            content.sourceComponent: DSliderRect {
                leftLabel: dsTr("Low")
                rightLabel: dsTr("High")

                value: (touchPadID.moveAccuracy - 1)/9
                realValue: touchPadID.moveAccuracy
                onValueChanged: {
                    if (touchPadID.moveAccuracy != 9 * value + 1){
                        touchPadID.moveAccuracy = 9 * value + 1
                    }
                }
            }
        }

        DCenterLine {
            height: contentHeight
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Double-click Frequency")
            content.sourceComponent: DSliderRect {
                leftLabel: dsTr("Slow")
                rightLabel: dsTr("Fast")
                width: sliderWidth

                value: (touchPadID.clickFrequency - 100)/900
                realValue: touchPadID.clickFrequency
                onValueChanged: {
                    if (touchPadID.clickFrequency != (900 * value + 100)){
                        touchPadID.clickFrequency = 900 * value + 100
                    }
                }
            }
        }

        DCenterLine {
            height: contentHeight
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Drag threshold")
            content.sourceComponent: DSliderRect {
                leftLabel: dsTr("Short")
                rightLabel: dsTr("Long")
                width: sliderWidth

                value: (touchPadID.dragDelay- 1)/9
                onValueChanged: {
                    if (touchPadID.dragDelay != (9 * value + 1)){
                        touchPadID.dragDelay = 9 * value + 1
                    }
                }
            }
        }

        DSeparatorHorizontal {}

    }
}
