import DBus.Com.Deepin.Daemon.ExtDevManager 1.0
import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import "../widgets"

Item {
    id: keyboardModule
    anchors.fill: parent

    property int contentLeftMargin: 22
    property int contentHeight: 60

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

    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Mouse")
        }

        DSepratorHorizontal {}

        DBaseHeader {
            height: 44
            title.sourceComponent: DssH2 {
                text: dsTr("Custom")
            }
            button.sourceComponent: DRadioButton {
				buttonModels: [{"buttonId": "left_hand", "buttonLabel": dsTr("Left Hand")}, 
							   {"buttonId": "right_hand", "buttonLabel": dsTr("Right Hand")}]

                onItemSelected: {
                    print(idx)
                }
            }
        }

        DSepratorHorizontal {}

        DBaseHeader {
            title.sourceComponent: DLabel {
                text: dsTr("Point Speed")
                font.pixelSize: 13
            }
        }

        DSepratorHorizontal {}

        DBaseHeader {
            color: dconstants.contentBgColor
            height: contentHeight
            leftMargin: contentLeftMargin
            title.sourceComponent: DLabel {
                text: dsTr("Acceleration")
                font.pixelSize: 12
            }
            button.sourceComponent: DSliderRect {
                leftLabel: dsTr("Slow")
                rightLabel: dsTr("Fast")

                //value: (2000 - keyboardID.repeatDelay)/1900
                onValueChanged: {
                    //if (keyboardID.repeatDelay != (2000 - 1900 * value)){
                        //keyboardID.repeatDelay = (2000 - 1900 * value)
                    //}
                }
            }
        }

        DSepratorHorizontal {}

        DBaseHeader {
            color: dconstants.contentBgColor
            height: contentHeight
            leftMargin: contentLeftMargin
            title.sourceComponent: DLabel {
                text: dsTr("Sensitivity")
                font.pixelSize: 12
            }
            button.sourceComponent: DSliderRect {
                leftLabel: dsTr("Low")
                rightLabel: dsTr("High")

                //value: (2000 - keyboardID.repeatDelay)/1900
                onValueChanged: {
                    //if (keyboardID.repeatDelay != (2000 - 1900 * value)){
                        //keyboardID.repeatDelay = (2000 - 1900 * value)
                    //}
                }
            }
        }

        DSepratorHorizontal {}

        DBaseHeader {
            title.sourceComponent: DssH2 {
                text: dsTr("Double-click")
            }
        }

        DSepratorHorizontal {}

        DBaseHeader {
            color: dconstants.contentBgColor
            height: contentHeight
            leftMargin: contentLeftMargin
            title.sourceComponent: DLabel {
                text: dsTr("Frequency")
                font.pixelSize: 12
            }
            button.sourceComponent: DSliderRect {
                leftLabel: dsTr("Slow")
                rightLabel: dsTr("Fast")

                //value: (2000 - keyboardID.repeatDelay)/1900
                onValueChanged: {
                    //if (keyboardID.repeatDelay != (2000 - 1900 * value)){
                        //keyboardID.repeatDelay = (2000 - 1900 * value)
                    //}
                }
            }
        }

        DSepratorHorizontal {}

        DBaseHeader {
            color: dconstants.contentBgColor
            height: contentHeight
            leftMargin: contentLeftMargin
            title.sourceComponent: DLabel {
                text: dsTr("Double click the smiling face to test")
                font.pixelSize: 12
            }
            button.sourceComponent: Image {
                id: smileyImage
                property string pic0: "images/smiley00.png"
                property string pic1: "images/smiley01.png"
                property bool opened: true
                scale: 0.8
                source: opened ? pic0 : pic1

                MouseArea {
                    anchors.fill: parent
                    onDoubleClicked: {
                        smileyImage.opened = !smileyImage.opened
                    }
                }
            }
        }

    }
}
