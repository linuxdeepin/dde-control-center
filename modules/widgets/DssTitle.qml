import QtQuick 2.0

Item {
    width: parent.width
    height: 48

    property alias text: title.text

    property var dconstants: DConstants {}

    DssH1 {
        id: title
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: dconstants.leftMargin
        font.bold: true
        style: Text.Raised
        styleColor: Qt.rgba(0, 0, 0, 0.9)
        color: "white"
    }
}
