import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.InputDevices 1.0
import "../shared"

Item {
    id: mouseModule
    anchors.fill: parent

    property int contentLeftMargin: 22
    property int contentHeight: 48
    property int sliderWidth: 180
    property int leftWidth: 100
    property int centerPadding: 16

    property var dbusMouse: Mouse {}
    property var touchpadObj

    Column {
        id: mouseSettingsColumn
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Mouse")

            rightLoader.sourceComponent: ResetButton {
                onClicked: {
                    dbusMouse.Reset()
                    if (isTouchpadExist){
                        touchpadObj.dbusTouchpad.Reset()
                    }
                }
            }
        }

        DSeparatorHorizontal {}

        DCenterLine {
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Primary Button")
            content.sourceComponent: DRadioButton {
                anchors.left: parent.left
                anchors.leftMargin: 2
                anchors.verticalCenter: parent.verticalCenter
                initializeIndex: dbusMouse.leftHanded ? 1 : 0
                buttonModel: [
                    {"buttonId": "right_hand", "buttonLabel": dsTr("Left-handed")},
                    {"buttonId": "left_hand", "buttonLabel": dsTr("Left")}
                ]

                onItemSelected: {
                    dbusMouse.leftHanded = idx == 1 ? true : false
                }
            }
        }

        DCenterLine {
            height: contentHeight
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Pointer Speed")

            content.sourceComponent: DSliderEnhanced {
                width: sliderWidth

                min: 0.5
                max: 5
                init: dbusMouse.motionAcceleration
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbusMouse.motionAcceleration = value
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
            title.text: dsTr("Pointer Precision")

            content.sourceComponent: DSliderEnhanced {
                width: sliderWidth

                min: 1
                max: 20
                init: dbusMouse.motionThreshold
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbusMouse.motionThreshold = value
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
            title.text: dsTr("Double-click Speed")

            content.sourceComponent: DSliderEnhanced {
                width: sliderWidth

                min: 100
                max: 1000
                init: dbusMouse.doubleClick
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbusMouse.doubleClick = value
                }

                Component.onCompleted: {
                    addRuler(100, dsTr("Slow"))
                    addRuler(1000, dsTr("Fast"))
                }
            }

        }

        DSeparatorHorizontal {}


        DBaseLine{ height: 26 }
        Item {
            id: touchpadBox
            width: parent.width
            height: childrenRect.height
            visible: isExist

            property var touchpadComponent: Qt.createComponent("TouchpadComponent.qml")
            property bool isExist: isTouchpadExist

            Component.onCompleted: {
                if(isExist){
                    touchpadObj = touchpadComponent.createObject(touchpadBox, {})
                    touchpadObj.x = touchpadBox.x
                    touchpadObj.y = touchpadBox.y
                }
            }
        }
    }
}
