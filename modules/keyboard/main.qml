import DBus.Com.Deepin.Daemon.ExtDevManager 1.0
import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import "../widgets/"

Item {
    id: keyboardModule
    anchors.fill: parent

    property int contentLeftMargin: 22

    ExtDevManager {
       id: "extDevManagerID"
       // path: "/com/deepin/daemon/ExtDevManager"
    } 
    Keyboard {
       id: "keyboardID"
       // path: "/com/deepin/daemon/ExtDevManager/Keyboard"
    } 
    Mouse {
       id: "mouseID"
       // path: "/com/deepin/daemon/ExtDevManager/Mouse"
    } 
    TouchPad {
       id: "touchPadID"
       // path: "/com/deepin/daemon/ExtDevManager/TouchPad"
    } 


    Text {
        id: keyboardModuleTitle
        anchors.top: parent.top
        anchors.topMargin: 16
        anchors.left: parent.left
        anchors.leftMargin: 20
        font.pixelSize: 15
        font.bold: true
        color: fgColor
        text: dsTr("Keyboard")
    }

    DSepratorHorizontal {
        anchors.top: parent.top
        anchors.topMargin: 46
    }

    Column {
        anchors.top: parent.top
        anchors.topMargin: 48
        width: parent.width

        DBaseHeader {
            title: DLabel {
                text: dsTr("Repeat")
                font.pixelSize: 13
            }
        }

        DBaseHeader {
            color: contentBgColor
            height: 60
            leftMargin: contentLeftMargin
            title: DLabel {
                text: dsTr("Repeat Delay")
                font.pixelSize: 12
            }
            button: DSlider {
                property real tmp: -1
                function getRepeatDelay(percent){
                    return 2000 - 1900 * percent
                }

                function getRepeatDelayPercent(interval){
                    return (2000 - interval)/1900
                }
                tmp: getRepeatDelayPercent(keyboardID.repeatDelay)
                onValueChanged: {
                    if (keyboardID.repeatDelay != getRepeatDelay(value)){
                        keyboardID.repeatDelay = getRepeatDelay(value)
                    }
                }
            }
        }

        DBaseHeader {
            color: contentBgColor
            height: 60
            leftMargin: contentLeftMargin
            title: DLabel {
                text: dsTr("Repeat Interval")
                font.pixelSize: 12
            }
            button: DSlider {
                onValueChanged: print(value)
            }
        }

        DBaseHeader {
            color: contentBgColor
            leftMargin: contentLeftMargin
            title: DLabel {
                text: dsTr("Test Repeat Interval")
                font.pixelSize: 13
            }
            button: TextInput {
                width: 100
                font.pixelSize: 12
                color: fgColor
                focus: true
            }
        }

        DBaseHeader {
            title: DLabel {
                text: dsTr("Cusor Blink")
                font.pixelSize: 13
            }
        }

        DBaseHeader {
            color: contentBgColor
            height: 60
            leftMargin: contentLeftMargin
            title: TextInput {
                width: 100
                font.pixelSize: 12
                color: fgColor
                focus: true
            }
            button: DSlider {
                onValueChanged: print(value)
            }
        }

        DBaseExpand {
            id: keyboardLayoutSetting
            header: DDownArrowHeader {
                text: dsTr("Keyboard Layout")
                onToggled: keyboardLayoutSetting.expanded = !keyboardLayoutSetting.expanded
            }

            content: Column {
                Text {text:"test"}
                Text {text:"test"}
                Text {text:"test"}
                Text {text:"test"}
                Text {text:"test"}
                Text {text:"test"}
                Text {text:"test"}
                DSepratorHorizontal {}
            }
        }
    }
}
