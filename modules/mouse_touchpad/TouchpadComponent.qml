import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.InputDevices 1.0

Item {
    id: keyboardModule
    width: parent.width
    height: childrenRect.height

    property int contentLeftMargin: 22
    property int contentHeight: 48
    property int sliderWidth: 180

    property var dbusTouchpad: TouchPad {}

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
                checked: dbusTouchpad.tPadEnable
                onClicked: {
                    dbusTouchpad.tPadEnable = checked
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
                initializeIndex: dbusTouchpad.leftHanded ? 1 : 0
                buttonModel: [
                    {"buttonId": "right_hand", "buttonLabel": dsTr("Right Hand")},
                    {"buttonId": "left_hand", "buttonLabel": dsTr("Left Hand")}
                ]

                onItemSelected: {
                    dbusTouchpad.leftHanded = (idx == 1)
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

                min: 0.5
                max: 5
                init: dbusTouchpad.motionAccel
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbusTouchpad.motionAccel = value
                }

                Component.onCompleted: {
                    addRuler(0.5, dsTr("Slow"))
                    addRuler(5, dsTr("Fast"))
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
                init: dbusTouchpad.motionThres
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbusTouchpad.motionThres = value
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
                init: dbusTouchpad.doubleClick
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbusTouchpad.doubleClick = value
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
                init: dbusTouchpad.dragThres
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbusTouchpad.dragThres = value
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
