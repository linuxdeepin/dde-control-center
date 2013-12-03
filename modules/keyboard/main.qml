import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import "../widgets/"

Item {
    id: keyboardModule
    anchors.fill: parent

    property int contentLeftMargin: 22

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

        DHeaderRect {
            title: DLabel {
                text: dsTr("Repeat")
                font.pixelSize: 13
            }
        }

        DHeaderRect {
            color: contentBgColor
            height: 60
            leftMargin: contentLeftMargin
            title: DLabel {
                text: dsTr("Repeat Delay")
                font.pixelSize: 12
            }
        }

        DHeaderRect {
            color: contentBgColor
            height: 60
            leftMargin: contentLeftMargin
            title: DLabel {
                text: dsTr("Repeat Interval")
                font.pixelSize: 12
            }
        }

        DHeaderRect {
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

        DHeaderRect {
            title: DLabel {
                text: dsTr("Cusor Blink")
                font.pixelSize: 13
            }
        }

        DHeaderRect {
            color: contentBgColor
            height: 60
            leftMargin: contentLeftMargin
            title: TextInput {
                width: 100
                font.pixelSize: 12
                color: fgColor
                focus: true
            }
        }

        DExpandRect {
            id: keyboardLayoutSetting
            header: DDownArrowHeader {
                text: dsTr("Keyboard Layout")
                onToggled: keyboardLayoutSetting.toggle()
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
