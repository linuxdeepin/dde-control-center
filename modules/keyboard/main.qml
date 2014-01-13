import DBus.Com.Deepin.Daemon.ExtDevManager 1.0
import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Locale 1.0
import Deepin.Widgets 1.0

Item {
    id: keyboardModule
    anchors.fill: parent

    property int contentLeftMargin: 22
    property int contentHeight: 60
    property int sliderWidth: 180

    property var dconstants: DConstants {}

    DLocale {
        id: xkeyboardLocale
        domain: "xkeyboard-config"
    }

    ExtDevManager {
       id: "extDevManagerID"
    } 
    Keyboard {
       id: "keyboardID"
    } 
    Mouse {
       id: "mouseID"
    } 
    TouchPad {
       id: "touchPadID"
    } 

    property var allLayoutList: keyboardID.LayoutList()

    Component {
        id: listModelComponent
        ListModel {}
    }

    Column {
        id: contentColumn
        anchors.top: parent.top
        width: parent.width
        height: childrenRect.height

        DssTitle { text:dsTr("Keyboard") }

        DSeparatorHorizontal {}

        DBaseLine {
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Repeat")
            }
        }

        DSeparatorHorizontal {}

        DBaseLine {
            color: dconstants.contentBgColor
            height: contentHeight
            leftMargin: contentLeftMargin
            leftLoader.sourceComponent: DLabel {
                text: dsTr("Repeat Delay")
                font.pixelSize: 12
            }
            rightLoader.sourceComponent: DSliderRect {
                width: sliderWidth
                leftLabel: dsTr("Long")
                rightLabel: dsTr("Short")

                value: (2000 - keyboardID.repeatDelay)/1900
                onValueChanged: {
                    if (keyboardID.repeatDelay != (2000 - 1900 * value)){
                        keyboardID.repeatDelay = (2000 - 1900 * value)
                    }
                }
            }
        }

        DSeparatorHorizontal {}

        DBaseLine {
            color: dconstants.contentBgColor
            height: contentHeight
            leftMargin: contentLeftMargin
            leftLoader.sourceComponent: DLabel {
                text: dsTr("Repeat Interval")
                font.pixelSize: 12
            }

            rightLoader.sourceComponent: DSliderRect {
                width: sliderWidth
                leftLabel: dsTr("Slow")
                rightLabel: dsTr("Fast")

                value: (2000 - keyboardID.repeatSpeed)/1980
                onValueChanged: {
                    if (keyboardID.repeatSpeed != (2000 - 1980 * value)){
                        keyboardID.repeatSpeed = 2000 - 1980 * value
                    }
                }
            }

        }

        DSeparatorHorizontal {}

        DBaseLine {
            color: dconstants.contentBgColor
            leftMargin: contentLeftMargin
            leftLoader.sourceComponent: DLabel {
                text: dsTr(" Test Repeat Interval")
                font.pixelSize: 13
            }
            rightLoader.sourceComponent: DTextInput {
                id: testRepeatIntervalInput
                width: 130
                //text: dsTr("Test repeat interval")
            }
        }

        DSeparatorHorizontal {}

        DBaseLine {
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Cusor Blink")
            }
        }

        DSeparatorHorizontal {}

        DBaseLine {
            color: dconstants.contentBgColor
            height: contentHeight
            leftMargin: contentLeftMargin
            leftLoader.sourceComponent: Rectangle {
                width: 1
                height: 14
                color: "white"

                Timer {
                    running: true
                    interval: keyboardID.cursorBlink/2
                    repeat: true
                    onTriggered: parent.visible = !parent.visible
                }
            }
            rightLoader.sourceComponent: DSliderRect{
                id: cursorBlinkSlider
                width: sliderWidth
                leftLabel: dsTr("Slow")
                rightLabel: dsTr("Fast")

                value: (2500 - keyboardID.cursorBlink)/2400
                onValueChanged: {
                    if (keyboardID.cursorBlink != (2500 - 2400 * value)){
                        keyboardID.cursorBlink = 2500 - 2400 * value
                    }
                }

            }
        }

        DSeparatorHorizontal {}

    }

    Column {
        width: parent.width
        height: childrenRect.height
        anchors.top: contentColumn.bottom
        anchors.left: contentColumn.left

        DBaseExpand {
            id: keyboardLayoutSetting
            property string defaultSelectItemId: keyboardID.keyboardLayout[0] ? keyboardID.keyboardLayout[0] : "us"
            property string currentLayoutName: xkeyboardLocale.dsTr(allLayoutList[defaultSelectItemId])
            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Keyboard Layout")
                hintText: "[" + keyboardLayoutSetting.currentLayoutName + "]"
                onClicked: {
                    keyboardLayoutSetting.expanded = !keyboardLayoutSetting.expanded
                }
            }

            content.sourceComponent: Component {
                ListView {
                    id: layoutList
                    width: parent.width
                    height: keyboardModule.height - contentColumn.height - 34

                    property var layoutKeyAndIndex: {
                        var obj = new Object();
                        var index = 0
                        for (var key in allLayoutList){
                            obj[key] = index
                            index += 1
                        }
                        return obj
                    }

                    model: {
                        var myModel = listModelComponent.createObject(parent, {})
                        for (var key in allLayoutList){
                            myModel.append({
                                "label": xkeyboardLocale.dsTr(allLayoutList[key]),
                                "item_id": key
                            })
                        }
                        return myModel
                    }

                    delegate: LayoutItem {}

                    highlight: Rectangle {
                        width: parent.width
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        height: 28
                        color: "#0D0D0D"
                        radius: 4
                    }
                    highlightMoveDuration: 100
                    focus: true
                    interactive: true

                    Component.onCompleted: {
                        positionViewAtIndex(layoutKeyAndIndex[keyboardLayoutSetting.defaultSelectItemId], ListView.center)
                    }

                    DScrollBar {
                        flickable: layoutList
                    }
                }


            }
        } // end DBaseExpand

        DSeparatorHorizontal {}
    }
}
