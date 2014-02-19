import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

Item {
    property alias text: label.text
    width: 90
    height: parent.height
    DssH2 {
        id: label
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
    }
}
