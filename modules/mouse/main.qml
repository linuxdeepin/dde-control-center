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

    property var mouseID: Mouse {}

    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Mouse")
        }

        DSeparatorHorizontal {}

        DBaseHeader {
            height: 44
            title.sourceComponent: DssH2 {
                text: dsTr("Custom")
            }
            button.sourceComponent: DRadioButton {
				buttonModel: [
					{"buttonId": "left_hand", "buttonLabel": dsTr("Left Hand")},
					{"buttonId": "right_hand", "buttonLabel": dsTr("Right Hand")}
				]

                onItemSelected: {
                    print(idx)
                }
            }
        }

        DSeparatorHorizontal {}

        DBaseHeader {
            title.sourceComponent: DssH2 {
                text: dsTr("Point Speed")
            }
        }

        DSeparatorHorizontal {}

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

                value: (mouseID.moveSpeed - 1)/9
                onValueChanged: {
                    if (mouseID.moveSpeed!= (9 * value + 1)){
                        mouseID.moveSpeed = 9 * value + 1
                    }
                }
            }
        }

        DSeparatorHorizontal {}

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

                value: (mouseID.moveAccuracy - 1)/9
                onValueChanged: {
                    if (mouseID.moveAccuracy != parseInt(9 * value + 2)){
                        mouseID.moveAccuracy = parseInt(9 * value + 2)
                    }
                }
            }
        }

        DSeparatorHorizontal {}

        DBaseHeader {
            title.sourceComponent: DssH2 {
                text: dsTr("Double-click")
            }
        }

        DSeparatorHorizontal {}

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

                value: (mouseID.clickFrequency - 100)/900
                onValueChanged: {
                    if (mouseID.clickFrequency != (900 * value + 100)){
                        mouseID.clickFrequency = 900 * value + 100
                    }
                }
            }
        }

        DSeparatorHorizontal {}

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

        DSeparatorHorizontal {}
    }
}
