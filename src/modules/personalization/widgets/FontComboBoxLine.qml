import QtQuick 2.1
import Deepin.Widgets 1.0
import QtQuick.Controls 1.0

Rectangle {
    id: header

    property alias title: title
    property int leftWidth: 120
    property int centerPadding: 10

    property alias boxWidth: menuBox.width
    property int menuMaxHeight: 200
    property alias parentWindow:menuBox.parentWindow
    property int defaultIndex: 0
    property var menuLabels

    signal menuSelect(int index)
    signal clicked()

    height: 30
    width: parent.width
    color: DPalette.bgColor

    DLabel {
        id: title
        font.pixelSize: 12
        anchors.verticalCenter: parent.verticalCenter
        width: leftWidth
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignRight
        text: "Double click frequency"
    }

    DComboBox {
        id: menuBox
        anchors.left: title.right
        anchors.leftMargin: centerPadding
        anchors.verticalCenter: title.verticalCenter

        parentWindow: header.parentWindow
        menu.maxHeight: menuMaxHeight
        width: 100
        onMenuSelect: {
            header.menuSelect(index)
        }
        onClicked: header.clicked()
        selectIndex: defaultIndex
        menu.model: menuLabels
        itemDelegate: FontComboBoxMenuItem {
            property int index
            property bool itemOnHover
            property var value

            text: value
        }
    }
}
