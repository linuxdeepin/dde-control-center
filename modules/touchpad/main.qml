import DBus.Com.Deepin.Daemon.ExtDevManager 1.0
import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import "../widgets"

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

        DssTitle {
            text: dsTr("TouchPad")
        }

        DSeparatorHorizontal {}

        DSwitchButtonHeader {
            text: dsTr("Enable touchpad")
            active: touchPadID.tPadEnable ? true : false

            onClicked: {
                touchPadID.tPadEnable = active
            }
        }

        DSeparatorHorizontal {}

        DBaseHeader {
            height: 44
            title.sourceComponent: DssH2 {
                text: dsTr("Custom")
            }
            button.sourceComponent: DRadioButton {
                currentIndex: touchPadID.useHabit == "right" ? 1 : 0
				buttonModel: [
					{"buttonId": "left_hand", "buttonLabel": dsTr("Left Hand")},
					{"buttonId": "right_hand", "buttonLabel": dsTr("Right Hand")}
				]

                onItemSelected: {
                    touchPadID.useHabit = idx == 1 ? "right" : "left"
                }
            }
        }

        DSeparatorHorizontal {}

        DBaseHeader {
            title.sourceComponent: DssH2 {
                text: dsTr("Point Speed")
            }
        }

        DSeparatorHorizontal {}

        DBaseHeader {
            color: dconstants.contentBgColor
            height: contentHeight
            leftMargin: contentLeftMargin
            title.sourceComponent: DLabel {
                text: dsTr("Acceleration")
                font.pixelSize: 12
            }
            button.sourceComponent: DSliderRect {
                width: sliderWidth
                leftLabel: dsTr("Slow")
                rightLabel: dsTr("Fast")

                value: (touchPadID.moveSpeed - 1)/9
                onValueChanged: {
                    if (touchPadID.moveSpeed!= (9 * value + 1)){
                        touchPadID.moveSpeed = 9 * value + 1
                    }
                }
            }
        }

        DSeparatorHorizontal {}

        DBaseHeader {
            color: dconstants.contentBgColor
            height: contentHeight
            leftMargin: contentLeftMargin
            title.sourceComponent: DLabel {
                text: dsTr("Sensitivity")
                font.pixelSize: 12
            }
            button.sourceComponent: DSliderRect {
                width: sliderWidth
                leftLabel: dsTr("Low")
                rightLabel: dsTr("High")

                value: (touchPadID.moveAccuracy - 1)/9
                onValueChanged: {
                    if (touchPadID.moveAccuracy != 9 * value + 1){
                        touchPadID.moveAccuracy = 9 * value + 1
                    }
                }
            }
        }

        DSeparatorHorizontal {}

        DBaseHeader {
            title.sourceComponent: DssH2 {
                text: dsTr("Double-click")
            }
        }

        DSeparatorHorizontal {}

        DBaseHeader {
            color: dconstants.contentBgColor
            height: contentHeight
            leftMargin: contentLeftMargin
            title.sourceComponent: DLabel {
                text: dsTr("Frequency")
                font.pixelSize: 12
            }
            button.sourceComponent: DSliderRect {
                width: sliderWidth
                leftLabel: dsTr("Slow")
                rightLabel: dsTr("Fast")

                value: (touchPadID.clickFrequency - 100)/900
                onValueChanged: {
                    if (touchPadID.clickFrequency != (900 * value + 100)){
                        touchPadID.clickFrequency = 900 * value + 100
                    }
                }
            }
        }

        DSeparatorHorizontal {}

        DBaseHeader {
            color: dconstants.contentBgColor
            height: contentHeight
            leftMargin: contentLeftMargin
            title.sourceComponent: DLabel {
                text: dsTr("Double click the smiling face to test")
                font.pixelSize: 12
            }
            button.sourceComponent: Image {
                id: smileyImage
                property string pic0: "images/smiley00.png"
                property string pic1: "images/smiley01.png"
                property bool opened: true
                scale: 0.8
                source: opened ? pic0 : pic1

                MouseArea {
                    anchors.fill: parent
                    onDoubleClicked: {
                        smileyImage.opened = !smileyImage.opened
                    }
                }
            }
        }

        DSeparatorHorizontal {}

        DBaseHeader {
            title.sourceComponent: DssH2 {
                text: dsTr("Drag and Drop")
            }
        }

        DSeparatorHorizontal {}

        DBaseHeader {
            color: dconstants.contentBgColor
            height: contentHeight
            leftMargin: contentLeftMargin
            title.sourceComponent: DLabel {
                text: dsTr("Drag threshold")
                font.pixelSize: 12
            }
            button.sourceComponent: DSliderRect {
                width: sliderWidth
                leftLabel: dsTr("Short")
                rightLabel: dsTr("Long")

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
