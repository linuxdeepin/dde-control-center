import DBus.Com.Deepin.Daemon.ExtDevManager 1.0
import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import "../widgets/"

Item {
    id: keyboardModule
    anchors.fill: parent

    property int contentLeftMargin: 22

    property var dconstants: DConstants {}

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
        color: dconstants.fgColor
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
            title.sourceComponent: DLabel {
                text: dsTr("Repeat")
                font.pixelSize: 13
            }
        }

        DBaseHeader {
            color: dconstants.contentBgColor
            height: 60
            leftMargin: contentLeftMargin
            title.sourceComponent: DLabel {
                text: dsTr("Repeat Delay")
                font.pixelSize: 12
            }
            button.sourceComponent: DSlider {
                function getRepeatDelay(percent){
                    return 2000 - 1900 * percent
                }

                value: (2000 - keyboardID.repeatDelay)/1900
                onValueChanged: {
                    if (keyboardID.repeatDelay != getRepeatDelay(value)){
                        keyboardID.repeatDelay = getRepeatDelay(value)
                    }
                }
            }
        }

        DBaseHeader {
            color: dconstants.contentBgColor
            height: 60
            leftMargin: contentLeftMargin
            title.sourceComponent: DLabel {
                text: dsTr("Repeat Interval")
                font.pixelSize: 12
            }
            button.sourceComponent: DSlider {
                onValueChanged: print(value)
            }
        }

        DBaseHeader {
            color: dconstants.contentBgColor
            leftMargin: contentLeftMargin
            title.sourceComponent: DLabel {
                text: dsTr("Test Repeat Interval")
                font.pixelSize: 13
            }
            button.sourceComponent: TextInput {
                width: 100
                font.pixelSize: 12
                color: dconstants.fgColor
                focus: true
            }
        }

        DBaseHeader {
            title.sourceComponent: DLabel {
                text: dsTr("Cusor Blink")
                font.pixelSize: 13
            }
        }

        DBaseHeader {
            color: dconstants.contentBgColor
            height: 60
            leftMargin: contentLeftMargin
            title.sourceComponent: TextInput {
                width: 100
                font.pixelSize: 12
                color: dconstants.fgColor
                focus: true
            }
            button.sourceComponent: DSlider {
                onValueChanged: print(value)
            }
        }

        DBaseExpand {
            id: keyboardLayoutSetting
            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Keyboard Layout")
                onClicked: keyboardLayoutSetting.expanded = !keyboardLayoutSetting.expanded
            }

            content.sourceComponent: Column {
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
