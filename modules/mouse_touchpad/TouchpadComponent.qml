import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.InputDevices 1.0

Item {
    id: keyboardModule
    anchors.fill: parent

    property int contentLeftMargin: 22
    property int contentHeight: 48
    property int sliderWidth: 180

    property var touchPadID: TouchPad {}

    Column {
        id: touchpadTitleColumn
        anchors.top: parent.top
        width: parent.width

        DBaseLine {
            id: touchpadTitle
            leftLoader.sourceComponent: DssH2 {
                color: "white"
                font.bold: true
                text: dsTr("TouchPad")
            }
            rightLoader.sourceComponent: DSwitchButton{
                checked: touchPadID.tPadEnable ? true : false
                onClicked: {
                    touchPadID.tPadEnable = checked
                }
            }
        }

        DSeparatorHorizontal {}
    }

    Column {
        id: touchpadSettingColumn
        anchors.top: touchpadTitleColumn.bottom
        width: parent.width
        clip: true
        property int realHeight: childrenRect.height

        height: touchpadTitle.rightLoader.item.checked ? realHeight : 0

        Behavior on height {
            PropertyAnimation { duration: 100 }
        }

        DCenterLine {
            height: 38
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Button Order")
            content.sourceComponent: DRadioButton {
                width: 180
                anchors.left: parent.left
                anchors.leftMargin: 2
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

            content.sourceComponent: DSliderEnhanced {
                width: sliderWidth

                min: 1
                max: 20
                init: touchPadID.moveSpeed
                valueDisplayVisible: false

                onValueConfirmed:{
                    touchPadID.moveSpeed = value
                }

                Component.onCompleted: {
                    addRuler(1, dsTr("Slow"))
                    addRuler(20, dsTr("Fast"))
                }
            }
        }

        DCenterLine {
            height: contentHeight
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Pointer Accuracy")

            content.sourceComponent: DSliderEnhanced {
                width: sliderWidth

                min: 1
                max: 20
                init: touchPadID.moveAccuracy
                valueDisplayVisible: false

                onValueConfirmed:{
                    touchPadID.moveAccuracy = value
                }

                Component.onCompleted: {
                    addRuler(1, dsTr("Low"))
                    addRuler(20, dsTr("High"))
                }
            }
        }

        DCenterLine {
            height: contentHeight
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Double-click Frequency")

            content.sourceComponent: DSliderEnhanced {
                width: sliderWidth

                min: 100
                max: 1000
                init: touchPadID.clickFrequency
                valueDisplayVisible: false

                onValueConfirmed:{
                    touchPadID.clickFrequency = value
                }

                Component.onCompleted: {
                    addRuler(100, dsTr("Slow"))
                    addRuler(1000, dsTr("Fast"))
                }
            }

        }

        DCenterLine {
            height: contentHeight
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Drag Threshold")

            content.sourceComponent: DSliderEnhanced {
                width: sliderWidth

                min: 1
                max: 10
                init: touchPadID.dragDelay
                valueDisplayVisible: false

                onValueConfirmed:{
                    touchPadID.dragDelay = value
                }

                Component.onCompleted: {
                    addRuler(1, dsTr("Short"))
                    addRuler(10, dsTr("Long"))
                }
            }
        }

        DSeparatorHorizontal {}
    }
}
