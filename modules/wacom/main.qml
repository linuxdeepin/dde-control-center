import QtQuick 2.1
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.InputDevices 1.0
import "../shared/"

Item {

    property int contentLeftMargin: 22
    property int contentHeight: 48
    property int sliderWidth: 170
    property int leftWidth: 100
    property int centerPadding: 16
    property var keyAction: {
            "PageUp": dsTr("Page Up"),
            "PageDown": dsTr("Page Down"),
            "LeftClick": dsTr("Left Click"),
            "RightClick": dsTr("Right Click"),
            "MiddleClick": dsTr("Middle Click")
        }
    property  var actionList: [
        keyAction["PageUp"],
        keyAction["PageDown"],
        keyAction["LeftClick"],
        keyAction["RightClick"],
        keyAction["MiddleClick"]
    ]


    DssTitle {
        id:wacomTitle
        anchors.top: parent.top
        text: modulesId.moduleLocaleNames["wacom"]
        rightLoader.sourceComponent: ResetButton {
            onClicked: {
                dbus_wacom.Reset()

                eraserModeDCL.content.children[0].currentIndex = dbus_wacom.leftHanded ? 1 : 0
                stylusModeDCL.content.children[0].currentIndex = dbus_wacom.cursorMode ? 1 : 0
                upKeyDCL.content.children[0].text = dbus_wacom.keyUpAction
                downKeyDCL.content.children[0].text = dbus_wacom.keyDownAction
                pressureDCL.content.children[0].setValue(dbus_wacom.pressureSensitive,false)
                twoclickSpaceDCL.content.children[0].setValue(dbus_wacom.doubleDelta,false)
            }
        }
    }

    DSeparatorHorizontal {
        id:wacomSeparator
        anchors.top: wacomTitle.bottom
        visible: dbus_wacom.exist
    }

    Column {
        id: wacomModule
        anchors.top: wacomSeparator.bottom
        anchors.topMargin: 8
        anchors.bottom: parent.bottom
        anchors.leftMargin: 20
        width: parent.width
        height: parent.height
    //    spacing: 8

        Wacom {id: dbus_wacom}

        DCenterLine {
            id: eraserModeDCL
            leftWidth: leftWidth
            centerPadding: centerPadding
            title.text: dsTr("Tablet Orientation")
            content.sourceComponent: DRadioButton {
                anchors.left: parent.left
                anchors.leftMargin: 2
                anchors.verticalCenter: parent.verticalCenter
                initializeIndex: dbus_wacom.leftHanded ? 1 : 0
                buttonModel: [
                    {"buttonId": "right_hand", "buttonLabel": dsTr("Left Hand")},
                    {"buttonId": "left_hand", "buttonLabel": dsTr("Right Hand")}
                ]

                onItemSelected: {
                    dbus_wacom.leftHanded = idx == 1 ? true : false
                }
            }
        }

        DCenterLine {
            id: stylusModeDCL
            leftWidth: leftWidth
            centerPadding: centerPadding
            title.text: dsTr("Digital Pen Modes")
            content.sourceComponent: DRadioButton {
                anchors.left: parent.left
                anchors.leftMargin: 2
                anchors.verticalCenter: parent.verticalCenter
                initializeIndex: dbus_wacom.cursorMode ? 1 : 0
                buttonModel: [
                    {"buttonId": "right_hand", "buttonLabel": dsTr("Pen Mode")},
                    {"buttonId": "left_hand", "buttonLabel": dsTr("Mouse Mode")}
                ]

                onItemSelected: {
                    dbus_wacom.cursorMode = idx == 1 ? true : false
                }
            }
        }

        DCenterLine {
            id:downKeyDCL
            height: contentHeight
            leftWidth: leftWidth
            centerPadding: centerPadding
            title.text: dsTr("Down Key")

            content.sourceComponent: DComboBox {
                parentWindow: rootWindow
                menu.maxHeight: 100
                onMenuSelect: {
                    for (var key in keyAction)
                    {
                        if (keyAction[key] == actionList[index])
                            dbus_wacom.keyDownAction = key
                    }
                }

                width: sliderWidth - 10
                labels: actionList
                selectIndex: actionList.indexOf(keyAction[dbus_wacom.keyDownAction])

            }
        }

        DCenterLine {
            id:upKeyDCL
            height: contentHeight
            leftWidth: leftWidth
            centerPadding: centerPadding
            title.text: dsTr("Up Key")

            content.sourceComponent: DComboBox {
                parentWindow: rootWindow
                menu.maxHeight: 100
                onMenuSelect: {
                    for (var key in keyAction)
                    {
                        if (keyAction[key] == actionList[index])
                            dbus_wacom.keyUpAction = key
                    }
                }

                width: sliderWidth - 10
                labels: actionList
                selectIndex: actionList.indexOf(keyAction[dbus_wacom.keyUpAction])

            }
        }

        DCenterLine {
            id:pressureDCL
            height: contentHeight
            leftWidth: leftWidth
            centerPadding: centerPadding
            title.text: dsTr("Pressure")

            content.sourceComponent: DSliderEnhanced {
                id: pressureSensitiveSlider
                width: sliderWidth

                min: 0
                max: 2047
                init: dbus_wacom.pressureSensitive
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbus_wacom.pressureSensitive = value
                }

                Component.onCompleted: {
                    addRuler(min, dsTr("Soft"))
                    addRuler(max, dsTr("Hard"))
                }
            }

        }

        DCenterLine {
            id:twoclickSpaceDCL
            height: contentHeight
            leftWidth: leftWidth
            centerPadding: centerPadding
            title.text: dsTr("Double-click Space")

            content.sourceComponent: DSliderEnhanced {
                id: doubleClickDetltaSlider
                width: sliderWidth

                min: 0
                max: 100
                init: dbus_wacom.doubleDelta
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbus_wacom.doubleDelta = value
                }

                Component.onCompleted: {
                    addRuler(min, dsTr("Narrow"))
                    addRuler(max, dsTr("Wide"))
                }
            }
        }
    }

}
