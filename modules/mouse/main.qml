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

    property var mouseID: Mouse {}

    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Mouse")

            rightLoader.sourceComponent: DTextButton {
                text: dsTr("Reset")
            }
        }

        DSeparatorHorizontal {}

        DBaseLine {
            height: 44
            leftMargin: contentLeftMargin + 3
            leftLoader.sourceComponent: DssH2 { text: dsTr("Button Order") }
            //leftLoader.sourceComponent: Item{
                //width: parent.height
                //height: parent.height
                //Image {
                    //anchors.centerIn: parent
                    //source: mouseID.useHabit ? "images/mouse_right.png" : "images/mouse_left.png"
                //}
            //}
            rightLoader.sourceComponent: DRadioButton {
                currentIndex: mouseID.useHabit ? 1 : 0
                buttonModel: [
                    {"buttonId": "right_hand", "buttonLabel": dsTr("Right Hand")},
                    {"buttonId": "left_hand", "buttonLabel": dsTr("Left Hand")}
                ]

                onItemSelected: {
                    mouseID.useHabit = idx == 1 ? true : false
                }
            }
        }

        DSeparatorHorizontal {}

        DBaseLine {}

        DBaseLine {
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Point Speed")
            }
        }

        DSeparatorHorizontal {}

        DBaseLine {
            height: contentHeight
            leftMargin: contentLeftMargin
            rightLoader.sourceComponent: Row {
                spacing: 10
                DLabel {
                    text: dsTr("Acceleration")
                    font.pixelSize: 12
                    anchors.verticalCenter: parent.verticalCenter
                }

                DSliderRect {
                    leftLabel: dsTr("Slow")
                    rightLabel: dsTr("Fast")

                    value: (mouseID.moveSpeed - 1)/9
                    realValue: mouseID.moveSpeed
                    onValueChanged: {
                        if (mouseID.moveSpeed!= (9 * value + 1)){
                            mouseID.moveSpeed = 9 * value + 1
                        }
                    }
                }
            }
        }

        DBaseLine {
            height: contentHeight
            leftMargin: contentLeftMargin
            rightLoader.sourceComponent: Row {
                spacing: 10

                DLabel {
                    text: dsTr("Sensitivity")
                    font.pixelSize: 12
                }

                DSliderRect {
                    leftLabel: dsTr("Low")
                    rightLabel: dsTr("High")

                    value: (mouseID.moveAccuracy - 1)/9
                    realValue: mouseID.moveAccuracy
                    onValueChanged: {
                        if (mouseID.moveAccuracy != 9 * value + 1){
                            mouseID.moveAccuracy = 9 * value + 1
                        }
                    }
                }
            }
        }

        DSeparatorHorizontal {}

        DBaseLine {}

        DBaseLine {
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Double-click")
            }
        }

        DSeparatorHorizontal {}

        DBaseLine {
            height: contentHeight
            leftMargin: contentLeftMargin
            rightLoader.sourceComponent: Row {
                spacing: 10

                DLabel {
                    text: dsTr("Frequency")
                    font.pixelSize: 12
                    anchors.verticalCenter: parent.verticalCenter
                }

                DSliderRect {
                    leftLabel: dsTr("Slow")
                    rightLabel: dsTr("Fast")

                    value: (mouseID.clickFrequency - 100)/900
                    realValue: mouseID.clickFrequency
                    onValueChanged: {
                        if (mouseID.clickFrequency != (900 * value + 100)){
                            mouseID.clickFrequency = 900 * value + 100
                        }
                    }
                }
            }
        }

        DSeparatorHorizontal {}

    }
}
