import QtQuick 2.1


Item {
    property int diameter: 18
    property int currentNumber: 0

    width: diameter + (String(currentNumber).length - 1) * defaultText.width
    height: diameter

    property var defaultText: NumberTipText {}

    Rectangle {
        anchors.fill: parent
        radius: diameter/2
        color: "red"
    }

    NumberTipText {
        anchors.centerIn: parent
        text: String(currentNumber)
    }
}
