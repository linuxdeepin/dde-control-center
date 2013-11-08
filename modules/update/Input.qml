import QtQuick 2.1

FocusScope {
    id:container
    width: 220
    height: 28
    BorderImage { source: "images/lineedit.sci"; anchors.fill: parent }
    property alias text: input.text
    property alias item:input
    TextInput{
        id: input
        width: parent.width - 12
        anchors.centerIn: parent
        font.pixelSize: 16;
        font.bold: true
        color: "#151515"; selectionColor: "mediumseagreen"
        focus: true
        onAccepted:{
            testDbus(container.text);
        }
        text: ""
        selectByMouse: true
    }
}
