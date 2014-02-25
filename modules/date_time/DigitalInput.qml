import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

TextInput {
    anchors.verticalCenter: parent.verticalCenter
    horizontalAlignment: TextInput.AlignHCenter
    width: implicitWidth + 4
    font.pixelSize: 55
    color: "white"
    selectByMouse: true

    property bool mouseAreaVisible: true

    MouseArea {
        anchors.fill: parent
        visible: mouseAreaVisible
        onDoubleClicked: {
            parent.selectAll()
            mouseAreaVisible = false
        }
    }
}
