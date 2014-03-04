import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.InputDevices 1.0

Item {
    id: mouseModule
    anchors.fill: parent

    property int contentLeftMargin: 22
    property int contentHeight: 60
    property int sliderWidth: 180
    property int leftWidth: 100
    property int centerPadding: 16

    property var mouseID: Mouse {}
    property var touchpadObj

    Column {
        id: mouseSettingsColumn
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Mouse")

            rightLoader.sourceComponent: DTextButton {
                text: dsTr("Reset")
            }
        }

        DSeparatorHorizontal {}

        DCenterLine {
            leftWidth: mouseModule.leftWidth
            centerPadding: mouseModule.centerPadding
            title.text: dsTr("Button Order")
            content.sourceComponent: DRadioButton {
                anchors.verticalCenter: parent.verticalCenter
                initializeIndex: mouseID.useHabit ? 1 : 0
                buttonModel: [
                    {"buttonId": "right_hand", "buttonLabel": dsTr("Right Hand")},
                    {"buttonId": "left_hand", "buttonLabel": dsTr("Left Hand")}
                ]

                onItemSelected: {
                    mouseID.useHabit = idx == 1 ? true : false
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

                value: (mouseID.moveSpeed - 1)/9
                realValue: mouseID.moveSpeed
                onValueChanged: {
                    if (mouseID.moveSpeed!= (9 * value + 1)){
                        mouseID.moveSpeed = 9 * value + 1
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

                value: (mouseID.moveAccuracy - 1)/9
                realValue: mouseID.moveAccuracy
                onValueChanged: {
                    if (mouseID.moveAccuracy != 9 * value + 1){
                        mouseID.moveAccuracy = 9 * value + 1
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

                value: (mouseID.clickFrequency - 100)/900
                realValue: mouseID.clickFrequency
                onValueChanged: {
                    if (mouseID.clickFrequency != (900 * value + 100)){
                        mouseID.clickFrequency = 900 * value + 100
                    }
                }
            }
        }

        DSeparatorHorizontal {}
    }

    Column {
        id: touchpadSettingsColumn
        anchors.top: mouseSettingsColumn.bottom
        anchors.topMargin: 30
        width: parent.width

        property var touchpadComponent: Qt.createComponent("TouchpadComponent.qml")
        property bool isExist: isTouchpadExist
        visible: isExist

        Item {
            id: touchpadBox
            width: parent.width
            height: childrenRect.height

            Component.onCompleted: {
                if(touchpadSettingsColumn.isExist){
                    touchpadObj = touchpadSettingsColumn.touchpadComponent.createObject(touchpadBox, {})
                    touchpadObj.x = touchpadBox.x
                    touchpadObj.y = touchpadBox.y
                }
            }
        }
    }
}
