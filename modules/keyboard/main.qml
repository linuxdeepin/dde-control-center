import DBus.Com.Deepin.Daemon.ExtDevManager 1.0
import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import "../widgets/"

Item {
    id: keyboardMod
    anchors.fill: parent

    property int contentLeftMargin: 22
    property int contentHeight: 60
    property int sliderWidth: 180

    property var dconstants: DConstants {}

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

    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text:dsTr("Keyboard")
        }

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
            rightLoader.sourceComponent: SliderRect {
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

            rightLoader.sourceComponent: SliderRect {
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
            rightLoader.sourceComponent: SliderRect{
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

        DBaseExpand {
            id: keyboardLayoutSetting
            property string layoutLabel
            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Keyboard Layout")
                hintText: "[" + keyboardLayoutSetting.layoutLabel + "]"
                onClicked: {
                    keyboardLayoutSetting.expanded = !keyboardLayoutSetting.expanded
                }
            }

            content.sourceComponent: Component {

                ListView {
                    id: layoutList
                    width: parent.width

                    property string defaultSelectItemId: keyboardID.keyboardLayout[0] ? keyboardID.keyboardLayout[0] : "us"

                    model: ListModel {}
                    delegate: Item {
                        width: parent.width
                        height: 28
                        anchors.left: parent.left
                        anchors.leftMargin: 25

                        property string itemId: item_id
                        
                        Row {
                            spacing: 5
                            anchors.verticalCenter: parent.verticalCenter
                            
                            Image {
                                id: nameImage
                                anchors.verticalCenter: parent.verticalCenter
                                source: "images/select.png"
                                opacity: layoutList.defaultSelectItemId == itemId ? 1 : 0
                            }
                            
                            DssH3 {
                                id: nameText
                                anchors.verticalCenter: parent.verticalCenter
                                text: label 
                                color: layoutList.defaultSelectItemId == itemId ? "#009EFF" : "#fff"
                                font.pixelSize: 12
                            }
                        }
                        
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            
                            onEntered: {
                                layoutList.currentIndex = index
                            }
                            
                            onClicked: {
                                layoutList.defaultSelectItemId = itemId
                                keyboardID.keyboardLayout = [itemId]
                                keyboardLayoutSetting.layoutLabel = allLayoutList[itemId]
                            }
                        }
                    }

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
                    highlightMoveDuration: 200
                    focus: true
                    interactive: true

                    Component.onCompleted: {
                        var length = 0;
                        var currentIndex = -1
                        for (var key in allLayoutList){
                            model.append({
                                "label": allLayoutList[key],
                                "item_id": key
                            })
                            if(key == layoutList.defaultSelectItemId){
                                currentIndex = length
                            }
                            length += 1
                        }
                        height = 150
                        if(currentIndex != -1){
                            layoutList.positionViewAtIndex(currentIndex, ListView.Visible)
                        }
                        keyboardLayoutSetting.layoutLabel = allLayoutList[layoutList.defaultSelectItemId]
                    }
                }
            }
        }
    }
}
